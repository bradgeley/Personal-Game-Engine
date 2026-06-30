// Bradley Christensen - 2022-2026
#include "SFlowField.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "Tile.h"
#include "TileDef.h"
#include "WorldSettings.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
IntVec2 neighborOffsets[4] = { IntVec2(1,  0),
                               IntVec2(-1,  0),
                               IntVec2(0,  1),
                               IntVec2(0, -1) };



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Startup()
{
    AddReadDependencies<SCWorld>();
    AddWriteDependencies<SCFlowField>();

    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();

    TagQuery tileTagQuery;
	tileTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsPath;
	tileTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsGoal;
	tileTagQuery.m_doesNotHaveAnyTags |= (uint8_t) TileTag::Solid;
	scFlowField.m_toGoalFlowField = FlowField(tileTagQuery);
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Shutdown() const
{
    SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
    scFlowField.m_toGoalFlowField.Reset();
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::Run(SystemContext const& context) const
{
    // Read Dependencies
    SCWorld const& world = context.GetSingletonConst<SCWorld>();

    if (!world.m_solidnessChanged)
    {
        return;
    }

	// Write Dependencies
    SCFlowField& scFlowField = context.GetSingleton<SCFlowField>();

    scFlowField.m_toGoalFlowField.Reset();

    SeedFlowField(scFlowField.m_toGoalFlowField, world);
    SetCostField(scFlowField.m_toGoalFlowField, world);
    GenerateFlow(scFlowField.m_toGoalFlowField, world);
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::SeedFlowField(FlowField& flowField, SCWorld const& world) const
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
void SFlowField::SetCostField(FlowField& flowField, SCWorld const& world) const
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
void SFlowField::GenerateFlow(FlowField& flowField, SCWorld const& world) const
{
    ScopedTimer timer("GenerateFlow");

    GenerateDistanceField(flowField, world);
    GenerateGradient(flowField, world);
	flowField.m_hasGeneratedFlow = true;
}



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateDistanceField(FlowField& flowField, SCWorld const& world) const
{
    ScopedTimer timer("- Generate Distance Field");

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
        if (!world.DoesTileMatchTagQuery(flowGenCoords.m_tileCoords, flowField.m_tileTagQuery))
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
            if (!world.DoesTileMatchTagQuery(neighborTileCoords, flowField.m_tileTagQuery))
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
                if (!world.DoesTileMatchTagQuery(nofnTileCoords, flowField.m_tileTagQuery))
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



//----------------------------------------------------------------------------------------------------------------------
void SFlowField::GenerateGradient(FlowField& flowField, SCWorld const& world) const
{
    ScopedTimer timer("- Generate Gradient");

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
        if (!world.DoesTileMatchTagQuery(flowGenCoords.m_tileCoords, flowField.m_tileTagQuery))
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

		bool isNorthValid = flowField.m_gradient.IsValidCoords(northTile) && world.DoesTileMatchTagQuery(northTile, flowField.m_tileTagQuery);
		bool isSouthValid = flowField.m_gradient.IsValidCoords(southTile) && world.DoesTileMatchTagQuery(southTile, flowField.m_tileTagQuery);
		bool isEastValid = flowField.m_gradient.IsValidCoords(eastTile) && world.DoesTileMatchTagQuery(eastTile, flowField.m_tileTagQuery);
		bool isWestValid = flowField.m_gradient.IsValidCoords(westTile) && world.DoesTileMatchTagQuery(westTile, flowField.m_tileTagQuery);

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
