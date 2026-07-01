// Bradley Christensen - 2022-2026
#include "STowerSpawner.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "SFlowField.h"
#include "WorldSettings.h"



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

    if (factory.m_towerPlacements.empty())
    {
        return;
	}

    if (CanPlaceTowers(factory, world))
    {
        for (TowerPlacementInfo const& placementInfo : factory.m_towerPlacements)
        {
            PlaceTowerInWorld(placementInfo, world);

            SpawnInfo spawnInfo;
            spawnInfo.m_spawnPos = placementInfo.m_worldPos;
            spawnInfo.m_def = EntityDef::GetEntityDef(placementInfo.m_towerName);
            SEntityFactory::SpawnEntity(context, spawnInfo);
        }
    }
	factory.m_towerPlacements.clear();
}



//----------------------------------------------------------------------------------------------------------------------
bool STowerSpawner::CanPlaceTowers(SCEntityFactory& factory, SCWorld const& world) const
{
    SCWorld copy = world;

    for (auto& placementInfo : factory.m_towerPlacements)
    {
        if (!PlaceTowerInWorld(placementInfo, copy))
        {
            return false;
        }
    }

    TagQuery tileTagQuery;
    tileTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsPath;
    tileTagQuery.m_hasAnyTags |= (uint8_t) TileTag::IsGoal;
    tileTagQuery.m_doesNotHaveAnyTags |= (uint8_t) TileTag::Solid;

    FlowField proxyWorldFlowField(tileTagQuery);

    SFlowField::SeedFlowField(proxyWorldFlowField, world);
	SFlowField::SetCostField(proxyWorldFlowField, copy);
	SFlowField::GenerateDistanceField(proxyWorldFlowField, copy);
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
                isFlowfieldValid = false;
                return false; // stop iterating
			}
        }
        return true; // keep iterating
	});

    return isFlowfieldValid;
}



//----------------------------------------------------------------------------------------------------------------------
bool STowerSpawner::PlaceTowerInWorld(TowerPlacementInfo const& placementInfo, SCWorld& world) const
{
    if (world.DoTilesInRegionMatchQuery(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, placementInfo.m_tileTagQuery))
    {
        bool solidnessOfPathTileChanged = false;

        world.ForEachPlayableTileInRegion(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, [&](IntVec2 const& worldCoords)
        {
            Tile& tile = world.m_tiles.GetRef(worldCoords);
            if (tile.IsPath() && !tile.IsSolid())
            {
                solidnessOfPathTileChanged = true;
            }
            tile.SetIsSolid(true);
            return true; // keep iterating
        });

        world.m_solidnessChanged |= solidnessOfPathTileChanged;

        return true;
    }

    return false;
}