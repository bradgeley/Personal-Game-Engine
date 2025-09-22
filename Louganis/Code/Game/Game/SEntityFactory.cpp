// Bradley Christensen - 2022-2025
#include "SEntityFactory.h"
#include "Engine/Core/XmlUtils.h"
#include "EntityDef.h"
#include "CTransform.h"
#include "SCEntityFactory.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Startup()
{
    AddWriteAllDependencies();

    EntityDef::LoadFromXML();
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Run(SystemContext const&)
{
    auto factory = g_ecs->GetComponent<SCEntityFactory>();

    // Destroy first
    for (auto& entToDestroy : factory->m_entitiesToDestroy)
    {
        g_ecs->DestroyEntity(entToDestroy);
    }
    factory->m_entitiesToDestroy.clear();

    // Spawn second
    for (SpawnInfo& spawnInfo : factory->m_entitiesToSpawn)
    {
        EntityID id = CreateEntityFromDef(spawnInfo.m_def);
        if (id == ENTITY_ID_INVALID)
        {
            continue;
		}

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



//----------------------------------------------------------------------------------------------------------------------
EntityID SEntityFactory::CreateEntityFromDef(EntityDef const* def)
{
	ASSERT_OR_DIE(def != nullptr, "Null entity definition passed to SEntityFactory::CreateEntityFromDef");

    EntityID id = g_ecs->CreateEntity();
    if (id == ENTITY_ID_INVALID)
    {
        DevConsoleUtils::LogError("Max entities (%i) reached, cannot spawn entity from definition: %s", MAX_ENTITIES, def->m_name.ToCStr());
        return ENTITY_ID_INVALID;
    }

    // Add components that exist in the def
    if (def->m_transform.has_value())           g_ecs->AddComponent<CTransform>(id);
    if (def->m_ai.has_value())                  g_ecs->AddComponent<CAIController>(id);
	if (def->m_ability.has_value())             g_ecs->AddComponent<CAbility>(id, *def->m_ability);
	if (def->m_animation.has_value())           g_ecs->AddComponent<CAnimation>(id, *def->m_animation);
    if (def->m_camera.has_value())              g_ecs->AddComponent<CCamera>(id, *def->m_camera);
	if (def->m_collision.has_value())           g_ecs->AddComponent<CCollision>(id, *def->m_collision);
	if (def->m_playerController.has_value())    g_ecs->AddComponent<CPlayerController>(id, *def->m_playerController);
    if (def->m_movement.has_value())            g_ecs->AddComponent<CMovement>(id, *def->m_movement);
    if (def->m_render.has_value())              g_ecs->AddComponent<CRender>(id, *def->m_render);

    return id;
}