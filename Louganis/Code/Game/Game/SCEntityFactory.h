// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"
#include <vector>



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
struct SpawnInfo
{
    Vec2 m_spawnPos;
    float m_spawnOrientation = 0.f;

    EntityDef const* m_def = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
struct SCEntityFactory
{
public:

    std::vector<SpawnInfo> m_entitiesToSpawn;
    std::vector<EntityID> m_entitiesToDestroy;
};
