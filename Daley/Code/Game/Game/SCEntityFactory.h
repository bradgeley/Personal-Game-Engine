// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include "SpawnInfo.h"
#include "TowerPlacementRequest.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct SCEntityFactory
{
public:

    std::vector<SpawnInfo> m_entitiesToSpawn;
    std::vector<EntityID> m_entitiesToDestroy;

	std::vector<TowerPlacementRequest> m_towerPlacements; // Managed by STowerSpawner
};
