// Bradley Christensen - 2022-2026
#include "STowerSpawner.h"
#include "Ability.h"
#include "AbilityDef.h"
#include "CAbility.h"
#include "EntityDef.h"
#include "FlavorDef.h"
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
bool AddFlavorToTower(EntityID entity, Name flavorName, SystemContext const& context);



//----------------------------------------------------------------------------------------------------------------------
void STowerSpawner::Startup()
{
	AddWriteAllDependencies(); // Spawns towers..

    DevConsoleUtils::AddDevConsoleCommand("FillMap", STowerSpawner::FillMapWithTower, "flavor", DevConsoleArgType::Name);
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

    // Tower Removal

    for (TowerRemovalRequest const& removalRequest : factory.m_towerRemovals)
    {
		CTransform const& transform = transformStorage[removalRequest.m_towerEntityID];
		CPlaceable const& placeable = placeableStorage[removalRequest.m_towerEntityID];

		if (!context.IsValid(removalRequest.m_towerEntityID))
		{
			continue;
		}

        if (CTags const* tags = context.GetComponent<CTags>(removalRequest.m_towerEntityID))
        {
            if (tags->HasTag("Obstacle"))
            {
				FloatingTextInstance floatingTextInstance;
				floatingTextInstance.m_lifetimeSeconds = 2.f;
				floatingTextInstance.m_pos = removalRequest.m_worldPos;
				floatingTextInstance.m_velocity = Vec2(0.f, 1.f);
				floatingTextInstance.m_text = "Cannot sell obstacles!";
				floatingTextInstance.m_tint = Rgba8::Red;
				scFloatingText.m_floatingTextInstances.push_back(floatingTextInstance);
                continue;
            }
        }

        if (runData.m_numSoldTowers == runData.m_maxSellsPerMission)
        {
            FloatingTextInstance floatingTextInstance;
            floatingTextInstance.m_lifetimeSeconds = 2.f;
            floatingTextInstance.m_pos = removalRequest.m_worldPos;
            floatingTextInstance.m_velocity = Vec2(0.f, 1.f);
            floatingTextInstance.m_text = "Sell limit reached!";
            floatingTextInstance.m_tint = Rgba8::Red;
            scFloatingText.m_floatingTextInstances.push_back(floatingTextInstance);
            continue;
        }

        runData.m_numSoldTowers++;

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


    // Tower Placement

    for (TowerPlacementRequest const& placementInfo : factory.m_towerPlacements)
    {
        TowerPlacementResult result = CanPlaceTower(placementInfo, world);
        if (result == TowerPlacementResult::Success)
        {
            PlaceTowerInWorld(placementInfo, world);

            SpawnInfo spawnInfo;
            spawnInfo.m_spawnPos = placementInfo.m_worldPos;
            spawnInfo.m_def = EntityDef::GetEntityDef(placementInfo.m_towerEntityName);
            EntityID tower = SEntityFactory::SpawnEntity(context, spawnInfo);

            if (context.IsValid(tower))
            {
                // Pass data to tower
                if (placementInfo.m_flavorName != Name::Invalid)
                {
                    AddFlavorToTower(tower, placementInfo.m_flavorName, context);
                }
				CPlaceable& placeableComp = *context.GetComponent<CPlaceable>(tower);
				placeableComp.m_botLeftTile = placementInfo.m_botLeftTileCoords;
				placeableComp.m_costOfPurchase = placementInfo.m_cost;
                runData.OnTowerPlacementSuccess(placementInfo);
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
bool AddFlavorToTower(EntityID tower, Name flavorName, SystemContext const& context)
{
	FlavorDef const* flavorDef = FlavorDef::GetFlavorDef(flavorName);
	ASSERT_OR_DIE(flavorDef, StringUtils::StringF("AddFlavorToTower: FlavorDef not found for flavor: %s", flavorName.ToCStr()).c_str());

	ASSERT_OR_DIE(context.IsValid(tower), StringUtils::StringF("AddFlavorToTower: Invalid tower entity: %u", tower).c_str());

	CTags* tags = context.GetComponent<CTags>(tower);
	ASSERT_OR_DIE(tags, StringUtils::StringF("AddFlavorToTower: CTags component not found for tower entity: %u", tower).c_str());

	tags->AddTag(flavorName);

	CAbility* abilityComp = context.GetComponent<CAbility>(tower);
	ASSERT_OR_DIE(abilityComp, StringUtils::StringF("AddFlavorToTower: CAbility component not found for tower entity: %u", tower).c_str());

    // Handle attributes
	abilityComp->m_attributes += flavorDef->m_attributes;

    // Flags
	abilityComp->m_abilityFlags.m_flags |= flavorDef->m_abilityFlags.m_flags;

    // Handle abilities
	for (auto& ability : flavorDef->m_abilities)
	{
		if (ability == Name::Invalid)
		{
            continue;
		}

        AbilityDef const* abilityDef = AbilityDef::GetAbilityDef(ability);
		ASSERT_OR_DIE(abilityDef, StringUtils::StringF("AddFlavorToTower: AbilityDef not found for ability: %s", ability.ToCStr()).c_str());

		Ability* abilityInstance = abilityDef->MakeAbilityInstance();
		abilityInstance->Initialize(context, tower);
		ASSERT_OR_DIE(abilityInstance, StringUtils::StringF("AddFlavorToTower: Failed to create Ability instance for ability: %s", ability.ToCStr()).c_str());

		abilityComp->m_abilities.push_back(abilityInstance);
	}

    // Handle cosmetics

	CAnimation* animComp = context.GetComponent<CAnimation>(tower);
	ASSERT_OR_DIE(animComp, StringUtils::StringF("AddFlavorToTower: CAnimation component not found for tower entity: %u", tower).c_str());

	animComp->m_spriteSheetName = flavorDef->m_spriteSheetName;
	animComp->m_defaultAnimationName = flavorDef->m_animName;

	CRender* renderComp = context.GetComponent<CRender>(tower);
	ASSERT_OR_DIE(renderComp, StringUtils::StringF("AddFlavorToTower: CRender component not found for tower entity: %u", tower).c_str());

	renderComp->m_baseTint = flavorDef->m_tint;
	renderComp->m_tint = flavorDef->m_tint;

	return true;
}


//----------------------------------------------------------------------------------------------------------------------
bool STowerSpawner::FillMapWithTower(NamedProperties& properties)
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCEntityFactory& factory = g_ecs->GetSingleton<SCEntityFactory>();

	Name towerEntityName = "Tower2x2"; // todo:
	Name towerFlavorName = properties.Get<Name>("flavor", Name("Vanilla"));

	world.ForEachVisibleTile([&](IntVec2 const& worldCoords, int)
	{
		TowerPlacementRequest placementInfo = SInput::MakeTowerPlacementRequest(towerEntityName, towerFlavorName, world.GetTileBounds(worldCoords).GetCenter(), world, true);
        placementInfo.m_isGenerated = true;
        factory.m_towerPlacements.push_back(placementInfo);
		return true; // keep iterating
	});


    return false;
}