// Bradley Christensen - 2023
#include "SEntityFactory.h"

#include "Game/Game/Components/CTransform.h"
#include "Game/Game/Singletons/SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Startup()
{
    AddWriteDependencies<SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Run(SystemContext const& context)
{
    auto factory = g_ecs->GetComponent<SCEntityFactory>();

    // Destroy first
    for (auto& entToDestroy : factory->m_entitiesToDestroy)
    {
        g_ecs->DestroyEntity(entToDestroy);
    }
    factory->m_entitiesToDestroy.clear();

    // Spawn second
    for (auto& spawnInfo : factory->m_entitiesToSpawn)
    {
        EntityID id = g_ecs->CreateEntityFromDef(*spawnInfo.m_def);
        CTransform* transform = g_ecs->GetComponent<CTransform>(id);
        if (transform)
        {
            transform->m_pos = spawnInfo.m_spawnPos;
            transform->m_orientation = spawnInfo.m_spawnOrientation;
        }
    }
    factory->m_entitiesToSpawn.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Shutdown()
{

}
