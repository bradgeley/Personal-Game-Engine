// Bradley Christensen - 2023
#include "SEntityFactory.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsole.h"
#include "Game/Game/EntityDef.h"
#include "Game/Game/Components/CTransform.h"
#include "Game/Game/Singletons/SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Startup()
{
    AddWriteDependencies<SCEntityFactory>();
    AddReadDependencies<CTransform>();
    
    LoadFromXml("Data/Definitions/EntityDefs.xml");
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
    for (SpawnInfo& spawnInfo : factory->m_entitiesToSpawn)
    {
        EntityID id = CreateEntityFromDef(spawnInfo.m_def);
        CTransform* transform = g_ecs->GetComponent<CTransform>(id);
        if (transform)
        {
            transform->m_pos = spawnInfo.m_spawnPos;
            transform->m_orientation = spawnInfo.m_spawnOrientation;
        }
        CPhysics* phys = g_ecs->GetComponent<CPhysics>(id);
        if (phys)
        {
            phys->m_velocity = spawnInfo.m_spawnVelocity;
        }
    }
    factory->m_entitiesToSpawn.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Shutdown()
{
    auto factory = g_ecs->GetComponent<SCEntityFactory>();

    for (auto& def : factory->m_entityDefinitions)
    {
        if (def.second)
        {
            def.second->Cleanup();
        }
    }

    factory->m_entityDefinitions.clear();
}



//----------------------------------------------------------------------------------------------------------------------
EntityID SEntityFactory::CreateEntityFromDef(EntityDef const* def) const
{
    EntityID result = g_ecs->CreateEntity();

    // Add components that exist in the def
    if (def->m_transform.has_value())           g_ecs->AddComponent<CTransform>(result);
    if (def->m_camera.has_value())              g_ecs->AddComponent<CCamera>(result, *def->m_camera);
    if (def->m_movement.has_value())            g_ecs->AddComponent<CMovement>(result, *def->m_movement);
    if (def->m_physics.has_value())             g_ecs->AddComponent<CPhysics>(result, *def->m_physics);
    if (def->m_render.has_value())              g_ecs->AddComponent<CRender>(result, *def->m_render);
	if (def->m_playerController.has_value())    g_ecs->AddComponent<CPlayerController>(result, *def->m_playerController);
	if (def->m_collision.has_value())           g_ecs->AddComponent<CCollision>(result, *def->m_collision);

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::LoadFromXml(char const* filename) const
{
    SCEntityFactory* scf = g_ecs->GetComponent<SCEntityFactory>();
    auto& entityDefs = scf->m_entityDefinitions;

    XmlDocument doc;
    doc.LoadFile(filename);
    auto root = doc.RootElement();
    if (!root)
    {
        g_devConsole->LogErrorF("SEntityFactory::LoadFromXml - Could not load file: %s", filename);
        return;
    }

    XmlElement* entityDefElem = root->FirstChildElement("Entity");
    while (entityDefElem)
    {
        std::string name = ParseXmlAttribute(*entityDefElem, "name", "Unnamed Entity Def");
        if (entityDefs.find(name) != entityDefs.end())
        {
            g_devConsole->LogErrorF("Duplicate Entity Def: %s", name.c_str());
        }

        // Create the definition and emplace it
        entityDefs.emplace(name, new EntityDef(entityDefElem));
        
        entityDefElem = entityDefElem->NextSiblingElement("Entity");
    }
}
