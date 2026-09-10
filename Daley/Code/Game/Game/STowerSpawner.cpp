// Bradley Christensen - 2022-2026
#include "STowerSpawner.h"
#include "CAbility.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "SCFloatingText.h"
#include "SCRunData.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "SFlowField.h"
#include "SInput.h"
#include "WorldSettings.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void STowerSpawner::Startup()
{
	AddWriteAllDependencies(); // Spawns towers..

    DevConsoleUtils::AddDevConsoleCommand("FillMap", STowerSpawner::FillMapWithTower, "tower", DevConsoleArgType::Name);
}



//----------------------------------------------------------------------------------------------------------------------
void STowerSpawner::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("FillMap", STowerSpawner::FillMapWithTower);
}



//----------------------------------------------------------------------------------------------------------------------
void STowerSpawner::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto const& placeableStorage = context.GetMapStorageConst<CPlaceable>();
	auto const& transformStorage = context.GetArrayStorageConst<CTransform>();

	// Write Dependencies
	SCEntityFactory& factory = context.GetSingleton<SCEntityFactory>();
    SCWorld& world = context.GetSingleton<SCWorld>();
	SCFloatingText& scFloatingText = context.GetSingleton<SCFloatingText>();
	SCRunData& scRunData = context.GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;

    for (TowerRemovalRequest const& removalRequest : factory.m_towerRemovals)
    {
		CTransform const& transform = transformStorage[removalRequest.m_towerEntityID];
		CPlaceable const& placeable = placeableStorage[removalRequest.m_towerEntityID];

		AABB2 towerBounds = AABB2(transform.m_pos, static_cast<float>(placeable.m_dims.x) * 0.5f, static_cast<float>(placeable.m_dims.y) * 0.5f);
		towerBounds.Squeeze(0.1f);

		world.ForEachPlayableTileOverlappingAABB(towerBounds, [&](IntVec2 const& worldCoords)
		{
			Tile tile = world.m_tiles.Get(worldCoords);
			tile.SetIsSolid(false);
			world.SetTile(worldCoords, tile);
            return true;
		});

        if (removalRequest.m_isSell)
        {
            float refund = placeable.m_costOfPurchase * runData.m_sellRefundRate;
			runData.m_gold += refund;

            FloatingTextInstance floatingTextInstance;
            floatingTextInstance.m_lifetimeSeconds = 2.f;
            floatingTextInstance.m_pos = transform.m_pos;
            floatingTextInstance.m_velocity = Vec2(0.f, 1.f);
            floatingTextInstance.m_text = StringUtils::StringF("+$%.1f (%i remaining)", refund, runData.m_maxSellsPerMission - runData.m_numSoldTowers);
            floatingTextInstance.m_tint = Rgba8::Green;
            floatingTextInstance.m_scale = 1.5f;
            scFloatingText.m_floatingTextInstances.push_back(floatingTextInstance);
        }

		CAbility* abilityComp = context.GetComponent<CAbility>(removalRequest.m_towerEntityID);
        if (abilityComp)
        {
            for (Ability* ability : abilityComp->m_abilities)
            {
				ability->Shutdown(context);
            }
        }

		context.DestroyEntity(removalRequest.m_towerEntityID);
    }

	factory.m_towerRemovals.clear();

    for (TowerPlacementRequest const& placementInfo : factory.m_towerPlacements)
    {
        TowerPlacementResult result = CanPlaceTower(placementInfo, world);
        if (result == TowerPlacementResult::Success)
        {
            PlaceTowerInWorld(placementInfo, world);

            SpawnInfo spawnInfo;
            spawnInfo.m_spawnPos = placementInfo.m_worldPos;
            spawnInfo.m_def = EntityDef::GetEntityDef(placementInfo.m_towerName);
            EntityID tower = SEntityFactory::SpawnEntity(context, spawnInfo);

            if (context.IsValid(tower))
            {
                // Pass data to tower
				CPlaceable& placeableComp = *context.GetComponent<CPlaceable>(tower);
				placeableComp.m_botLeftTile = placementInfo.m_botLeftTileCoords;
				placeableComp.m_costOfPurchase = placementInfo.m_cost;
            }

			runData.m_gold -= placementInfo.m_cost;
        }
        else if (!placementInfo.m_isGenerated)
        {
            FloatingTextInstance floatingTextInstance;
            floatingTextInstance.m_pos = placementInfo.m_worldPos;
            floatingTextInstance.m_lifetimeSeconds = 2.f;
            floatingTextInstance.m_velocity = Vec2(0.f, 1.f);
            floatingTextInstance.m_scale = 1.f;
            floatingTextInstance.m_tint = Rgba8::Red;

			if (result == TowerPlacementResult::Blocked)
			{
				floatingTextInstance.m_text = "Blocked!";
			}
			else if (result == TowerPlacementResult::BlocksPath)
			{
				floatingTextInstance.m_text = "Cannot block path!";
			}
			else if (result == TowerPlacementResult::CannotAfford)
			{
				floatingTextInstance.m_text = "Cannot Afford!";
			}
			else if (result == TowerPlacementResult::OffVisibleMap)
			{
				floatingTextInstance.m_text = "Cannot build there!";
			}

            scFloatingText.m_floatingTextInstances.push_back(floatingTextInstance);
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

    IntVec2 tileCoords;
	for (tileCoords.x = info.m_botLeftTileCoords.x; tileCoords.x <= info.m_topRightTileCoords.x; ++tileCoords.x)
	{
		for (tileCoords.y = info.m_botLeftTileCoords.y; tileCoords.y <= info.m_topRightTileCoords.y; ++tileCoords.y)
		{
			if (!world.IsTileVisible(tileCoords))
			{
				return TowerPlacementResult::OffVisibleMap;
			}
		}
	}

    SCWorld copy;
	copy.m_cachedSpawnLocations = world.m_cachedSpawnLocations;
	copy.m_tiles = world.m_tiles;
    if (!info.m_isGenerated)
    {
        copy.m_numEnemiesInTile = world.m_numEnemiesInTile;
    }

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

	copy.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		// Check if there is an enemy in this tile, and if so, we cannot place a tower here
		// Also, if this tower would block pathing to a tile with an enemy in it, we cannot place a tower here
        if (!copy.IsTileOnPath(tileCoords))
        {
            return true;
        }

        int numEnemies = info.m_isGenerated ? 0 : copy.m_numEnemiesInTile.Get(tileCoords);
        if (numEnemies > 0)
        {
		    float distanceToTileWithEnemy = proxyWorldFlowField.GetDistanceAtTileCoords(tileCoords);
			if (distanceToTileWithEnemy == StaticWorldSettings::s_maximumFlowDistance)
			{
				isFlowfieldValid = false;
				return false; // stop iterating
			}
        }

        return true;
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
			world.SetTile(worldCoords, tile);
            return true; // keep iterating
        });

        return true;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool STowerSpawner::WillChangePathSolidness(TowerPlacementRequest const& placementInfo, SCWorld const& world) const
{
	bool isValidPlacement = world.DoTilesInRegionMatchQuery(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, placementInfo.m_tileTagQuery);
	if (!isValidPlacement)
    {
        return false;
    }

	bool willChangePathSolidness = false;
    world.ForEachPlayableTileInRegion(placementInfo.m_botLeftTileCoords, placementInfo.m_topRightTileCoords, [&](IntVec2 const& worldCoords)
    {
        // m_solidnessOfPathTileChanged |= (tile.IsPath() != existingTile.IsPath()) || (tile.IsPath() && tile.IsSolid() != existingTile.IsSolid());
        Tile const& tile = world.m_tiles.Get(worldCoords);
		if (tile.IsPath() && !tile.IsSolid())
        {
            willChangePathSolidness = true;
            return false; // stop iterating
        }
        return true; // keep iterating
    });

    return willChangePathSolidness;
}



//----------------------------------------------------------------------------------------------------------------------
bool STowerSpawner::FillMapWithTower(NamedProperties& properties)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCEntityFactory& factory = g_ecs->GetSingleton<SCEntityFactory>();

	Name towerName = properties.Get<Name>("tower", Name("Vanilla"));

	world.ForEachVisibleTile([&](IntVec2 const& worldCoords, int)
	{
		TowerPlacementRequest placementInfo = SInput::MakeTowerPlacementRequest(towerName, world.GetTileBounds(worldCoords).GetCenter(), world);
        placementInfo.m_isGenerated = true;
        factory.m_towerPlacements.push_back(placementInfo);
		return true; // keep iterating
	});


    return false;
}
