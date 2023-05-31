// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"
#include <unordered_map>
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
    EntityDef const* GetEntityDef(std::string const& name) const;
    
    std::vector<SpawnInfo> m_entitiesToSpawn;
    std::vector<EntityID> m_entitiesToDestroy;

protected:

    friend class SEntityFactory;
    
    std::unordered_map<std::string, EntityDef*> m_entityDefinitions;
};
