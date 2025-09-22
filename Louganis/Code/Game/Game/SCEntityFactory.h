// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/EntityID.h"
#include "SpawnInfo.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct SCEntityFactory
{
public:

    std::vector<SpawnInfo> m_entitiesToSpawn;
    std::vector<EntityID> m_entitiesToDestroy;
};
