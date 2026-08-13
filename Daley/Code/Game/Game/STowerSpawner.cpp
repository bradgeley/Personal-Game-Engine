// Bradley Christensen - 2022-2026
#include "STowerSpawner.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "SCFloatingText.h"
#include "SCRunData.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "SFlowField.h"
#include "WorldSettings.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void STowerSpawner::Startup()
{
	AddWriteAllDependencies(); // Spawns towers..
}



//----------------------------------------------------------------------------------------------------------------------
void STowerSpawner::Shutdown() const
{
}



//----------------------------------------------------------------------------------------------------------------------
void STowerSpawner::Run(SystemContext const& context) const
{
	// Write Dependencies
	SCEntityFactory& factory = context.GetSingleton<SCEntityFactory>();
    SCWorld& world = context.GetSingleton<SCWorld>();
	SCFloatingText& scFloatingText = context.GetSingleton<SCFloatingText>();
	SCRunData& runData = context.GetSingleton<SCRunData>();

    if (factory.m_towerPlacements.empty())
    {
        return;
	}

    for (TowerPlacementRequest const& placementInfo : factory.m_towerPlacements)
    {
        TowerPlacementResult result = CanPlaceTower(placementInfo, world);
        if (result == TowerPlacementResult::Success)
        {
            PlaceTowerInWorld(placementInfo, world);

            SpawnInfo spawnInfo;
            spawnInfo.m_spawnPos = placementInfo.m_worldPos;
            spawnInfo.m_def = EntityDef::GetEntityDef(placementInfo.m_towerName);
            SEntityFactory::SpawnEntity(context, spawnInfo);

			runData.m_gold -= placementInfo.m_cost;
        }
        else if (!placementInfo.m_isGenerated)
        {
            FloatingTextInstance floatingText;
            floatingText.m_pos = placementInfo.m_worldPos;
            floatingText.m_lifetimeSeconds = 2.f;
            floatingText.m_velocity = Vec2(0.f, 1.f);
            floatingText.m_scale = 1.f;

			if (result == TowerPlacementResult::Blocked)
			{
				floatingText.m_text = "Blocked!";
				floatingText.m_tint = Rgba8::Red;
			}
			else if (result == TowerPlacementResult::BlocksPath)
			{
				floatingText.m_text = "Cannot block path!";
				floatingText.m_tint = Rgba8::Red;
			}
			else if (result == TowerPlacementResult::CannotAfford)
			{
				floatingText.m_text = "Cannot Afford!";
				floatingText.m_tint = Rgba8::Red;
			}

            scFloatingText.m_floatingTextInstances.push_back(floatingText);
        }
    }

	factory.m_towerPlacements.clear();
}



//----------------------------------------------------------------------------------------------------------------------
TowerPlacementResult STowerSpawner::CanPlaceTower(TowerPlacementRequest const& info, SCWorld const& world) const
{
	if (info.m_canAfford == false)
	{
		return TowerPlacementResult::CannotAfford;
	}

    SCWorld copy = world;

    copy.m_solidnessOfPathTileChanged = false;

    if (!PlaceTowerInWorld(info, copy))
    {
        return TowerPlacementResult::Blocked;
    }

    if (!copy.m_solidnessOfPathTileChanged)
    {
        return TowerPlacementResult::Success;
    }

    TagQuery tileTagQuery;
    tileTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsPath;
    tileTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsGoal;
    tileTagQuery.m_doesNotHaveAnyTags |= (uint8_t) TileTag::Solid;

    FlowField proxyWorldFlowField(tileTagQuery);

    SFlowField::SeedFlowField(proxyWorldFlowField, world);
    SFlowField::SetCostField(proxyWorldFlowField, copy);
    SFlowField::GenerateDistanceField(proxyWorldFlowField, copy, true);
    proxyWorldFlowField.m_hasGeneratedFlow = true;

    bool isFlowfieldValid = true;
    copy.ForEachCachedEdgePathTile([&](IntVec2 const& tileCoords)
    {
        Tile const& tile = copy.m_tiles.Get(tileCoords);
        if (tile.IsPath())
        {
            float distanceOnEdgePath = proxyWorldFlowField.GetDistanceAtTileCoords(tileCoords);
            if (distanceOnEdgePath == StaticWorldSettings::s_maximumFlowDistance)
            {
				// If a path edge tile is blocked, then the flowfield is invalid and the tower placement is invalid
                isFlowfieldValid = false;
                return false; // stop iterating
            }
        }
        return true; // keep iterating
    });

    return isFlowfieldValid ? TowerPlacementResult::Success : TowerPlacementResult::BlocksPath;
}



//----------------------------------------------------------------------------------------------------------------------
bool STowerSpawner::PlaceTowerInWorld(TowerPlacementRequest const& placementInfo, SCWorld& world) const
{
    if (world.DoTilesInRegionMatchQuery(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, placementInfo.m_tileTagQuery))
    {
        world.ForEachPlayableTileInRegion(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, [&](IntVec2 const& worldCoords)
        {
            Tile tile = world.m_tiles.Get(worldCoords);
			tile.SetIsSolid(true);
            tile.SetIsOpaque(true);
			world.SetTile(worldCoords, tile);
            return true; // keep iterating
        });

        return true;
    }

    return false;
}