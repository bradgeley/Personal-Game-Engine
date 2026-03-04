// Bradley Christensen - 2022-2026
#include "SFlowField.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "TileDef.h"
#include "WorldSettings.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include <cfloat>
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
IntVec2 neighborOffsets[4] = { IntVec2(1,  0),
                               IntVec2(-1,  0),
                               IntVec2(0,  1),
                               IntVec2(0, -1) };



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Startup()
{
    AddReadDependencies<CTransform, SCWorld>();
    AddWriteDependencies<SCFlowField>();
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Run(SystemContext const&)
{
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    if (scFlowField.m_toGoalFlowField.m_hasGeneratedFlow)
    {
        return;
	}

    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

    SeedFlowField(scFlowField.m_toGoalFlowField, world);
    SetCostField(scFlowField.m_toGoalFlowField, world);
    GenerateFlow(scFlowField.m_toGoalFlowField);
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Shutdown()
{
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    scFlowField.m_toGoalFlowField.Reset();
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::SeedFlowField(FlowField& flowField, SCWorld const& world)
{
    for (int y = 0; y <= StaticWorldSettings::s_playableWorldEndIndexY; ++y)
    {
        for (int x = 0; x <= StaticWorldSettings::s_playableWorldEndIndexX; ++x)
        {
            IntVec2 currentCoords = IntVec2(x, y);
            if (world.m_tiles.Get(currentCoords).IsGoal())
            {
                flowField.SeedUnsafe(currentCoords);
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::SetCostField(FlowField& flowField, SCWorld const& world)
{
    int index = 0; // trick only works if iterating from 0,0
    for (int y = 0; y <= StaticWorldSettings::s_playableWorldEndIndexY; ++y)
    {
        for (int x = 0; x <= StaticWorldSettings::s_playableWorldEndIndexX; ++x, ++index)
        {
            uint8_t cost = world.GetTileCost(index);
            flowField.m_costField.Set(index, cost);
        }
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateFlow(FlowField& flowField)
{
    ScopedTimer timer("GenerateFlow");

    GenerateDistanceField(flowField);
    GenerateGradient(flowField);
	flowField.m_hasGeneratedFlow = true;
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateDistanceField(FlowField& flowField)
{
    ScopedTimer timer("- Generate Distance Field");
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

    flowField.AddSeedsToOpenList();

    while (!flowField.m_openList.empty())
    {
        FlowGenerationCoords flowGenCoords = flowField.m_openList.top();
        flowField.m_openList.pop();

        IntVec2 const& currentTileCoords = flowGenCoords.m_tileCoords;

        int currentIndex = flowField.m_distanceField.GetIndexForCoords(currentTileCoords);

        if (flowField.m_consideredCells.Get(currentIndex))
        {
            continue;
        }
        if (!world.IsTileOnPath(flowGenCoords.m_tileCoords))
        {
            continue;
        }
        flowField.m_consideredCells.Set(currentIndex);

        for (IntVec2 const& neighborOffset : neighborOffsets)
        {
            IntVec2 neighborTileCoords = flowGenCoords.m_tileCoords + neighborOffset;
            if (!flowField.m_distanceField.IsValidCoords(neighborTileCoords))
            {
                continue;
			}
            int neighborLocalTileIndex = flowField.m_distanceField.GetIndexForCoords(neighborTileCoords);
            if (flowField.m_consideredCells.Get(neighborLocalTileIndex))
            {
                continue;
            }
            if (!world.IsTileOnPath(neighborTileCoords))
            {
                continue;
            }

            int neighborCost = (int) flowField.m_costField.Get(neighborTileCoords);

            float dx = FLT_MAX; // dx = min( neighborWestDistance, neighborEastDistance );
            float dy = FLT_MAX; // dy = min( neighborNorthDistance, neighborSouthDistance );

            // nofn = neighbor of neighbor
            for (IntVec2 const& nofnOffset : neighborOffsets)
            {
                IntVec2 nofnTileCoords = neighborTileCoords + nofnOffset;
                if (!flowField.m_distanceField.IsValidCoords(nofnTileCoords))
                {
                    continue;
                }
                if (!world.IsTileOnPath(nofnTileCoords))
                {
                    continue;
                }
                float nofnDistance = flowField.m_distanceField.Get(nofnTileCoords);
                if (nofnOffset.x != 0 && dx > nofnDistance)
                {
                    dx = nofnDistance; // get the minimum of east/west distances
                }
                else if (nofnOffset.y != 0 && dy > nofnDistance)
                {
                    dy = nofnDistance; // get the minimum of north/south distances
                }
            }

            // Delta = 2 * neighborCost - (dx-dy)^2;
            // if Delta >= 0
            //     D(j) = (dx + dy + sqrt(Delta)) / 2;
            // else
            //     D(j) = min(dx + W(j), dy + W(j));
            // end

            float delta = 2 * neighborCost - MathUtils::PowF(dx - dy, 2);
            float calculatedNeighborDistance;
            if (delta >= 0)
            {
                calculatedNeighborDistance = (dx + dy + MathUtils::SqrtF(delta)) / 2.f;
            }
            else
            {
                calculatedNeighborDistance = MathUtils::Min(dx + neighborCost, dy + neighborCost);
            }

            if (calculatedNeighborDistance < flowField.m_distanceField.Get(neighborTileCoords))
            {
                flowField.m_distanceField.Set(neighborTileCoords, calculatedNeighborDistance);
                flowField.m_openList.push({ neighborTileCoords, calculatedNeighborDistance });
            }
        }
    }
}

void SFlowField::GenerateGradient(FlowField& flowField)
{
    ScopedTimer timer("- Generate Gradient");
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

    flowField.ResetConsideredCells();

    flowField.AddSeedsToOpenList();

    while (!flowField.m_openList.empty())
    {
        FlowGenerationCoords flowGenCoords = flowField.m_openList.top();
        flowField.m_openList.pop();

        IntVec2 const& currentTileCoords = flowGenCoords.m_tileCoords;
        int currentTileIndex = flowField.m_gradient.GetIndexForCoords(flowGenCoords.m_tileCoords);

        if (flowField.m_consideredCells.Get(currentTileIndex))
        {
            continue;
        }
        if (!world.IsTileOnPath(flowGenCoords.m_tileCoords))
        {
            continue;
        }

        flowField.m_consideredCells.Set(currentTileIndex);

        float currentTileDistance = flowField.m_distanceField.Get(currentTileCoords);

        // Boundary aware differentiation
		IntVec2 northTile = currentTileCoords + IntVec2(0, 1);
		IntVec2 southTile = currentTileCoords + IntVec2(0, -1);
		IntVec2 eastTile = currentTileCoords + IntVec2(1, 0);
		IntVec2 westTile = currentTileCoords + IntVec2(-1, 0);

		bool isNorthValid = flowField.m_gradient.IsValidCoords(northTile) && world.IsTileOnPath(northTile);
		bool isSouthValid = flowField.m_gradient.IsValidCoords(southTile) && world.IsTileOnPath(southTile);
		bool isEastValid = flowField.m_gradient.IsValidCoords(eastTile) && world.IsTileOnPath(eastTile);
		bool isWestValid = flowField.m_gradient.IsValidCoords(westTile) && world.IsTileOnPath(westTile);

		float northDistance = isNorthValid ? flowField.m_distanceField.Get(northTile) : currentTileDistance;
		float southDistance = isSouthValid ? flowField.m_distanceField.Get(southTile) : currentTileDistance;
		float eastDistance = isEastValid ? flowField.m_distanceField.Get(eastTile) : currentTileDistance;
		float westDistance = isWestValid ? flowField.m_distanceField.Get(westTile) : currentTileDistance;

        float dY = northDistance - southDistance;
		float dX = eastDistance - westDistance;

        Vec2& gradient = flowField.m_gradient.GetRef(currentTileCoords);
        if (MathUtils::IsNearlyZero(dX) && MathUtils::IsNearlyZero(dY))
        {
            gradient = Vec2::ZeroVector;
        }
        else
        {
            gradient = -1.f * Vec2(dX, dY).GetNormalized();
		}

        int northIndex = flowField.m_gradient.GetIndexForCoords(northTile);
        if (isNorthValid && !flowField.m_consideredCells.Get(northIndex))
        {
            flowField.m_openList.push({ northTile, northDistance });
        }

		int southIndex = flowField.m_gradient.GetIndexForCoords(southTile);
        if (isSouthValid && !flowField.m_consideredCells.Get(southIndex))
        {
            flowField.m_openList.push({ southTile, southDistance });
		}

		int eastIndex = flowField.m_gradient.GetIndexForCoords(eastTile);
        if (isEastValid && !flowField.m_consideredCells.Get(eastIndex))
        {
            flowField.m_openList.push({ eastTile, eastDistance });
        }

		int westIndex = flowField.m_gradient.GetIndexForCoords(westTile);
        if (isWestValid && !flowField.m_consideredCells.Get(westIndex))
        {
            flowField.m_openList.push({ westTile, westDistance });
		}
    }
}
