// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/ECS/EntityDefinition.h"
#include "Engine/ECS/Component.h"
#include "Engine/Math/Vec2.h"
#include <unordered_map>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct SpawnInfo
{
    Vec2 m_spawnPos;
    float m_spawnOrientation = 0.f;
    EntityDefinition* m_def = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
struct SCEntityFactory : Component
{
    std::unordered_map<std::string, EntityDefinition> m_entityDefinitions;
    std::vector<SpawnInfo>          m_entitiesToSpawn;
    std::vector<EntityID>           m_entitiesToDestroy;
};
