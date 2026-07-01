// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include "SpawnInfo.h"
#include "TowerPlacementInfo.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct SCEntityFactory
{
public:

    std::vector<SpawnInfo> m_entitiesToSpawn;
    std::vector<EntityID> m_entitiesToDestroy;

	std::vector<TowerPlacementInfo> m_towerPlacements; // Managed by STowerSpawner
};
