// Bradley Christensen - 2022-2026
#include "SEntityFactory.h"
#include "EntityDef.h"
#include "CTransform.h"
#include "SCEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Startup()
{
    AddWriteAllDependencies();

    EntityDef::LoadFromXML();
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Shutdown() const
{
    EntityDef::Shutdown();
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityFactory::Run(SystemContext const& context) const
{
    SCEntityFactory& factory = context.GetSingleton<SCEntityFactory>();

    // Destroy first
    for (auto& entToDestroy : factory.m_entitiesToDestroy)
    {
        context.DestroyEntity(entToDestroy);
    }
    factory.m_entitiesToDestroy.clear();

    // Spawn second
    for (SpawnInfo& spawnInfo : factory.m_entitiesToSpawn)
    {
        EntityID id = SpawnEntity(context, spawnInfo);
        if (id == EntityID::Invalid)
        {
            continue;
		}
    }
    factory.m_entitiesToSpawn.clear();
}



//----------------------------------------------------------------------------------------------------------------------
EntityID SEntityFactory::CreateEntityFromDef(SystemContext const& context, EntityDef const* def)
{
    if (!context.HasFullECSAccess())
    {
        ERROR_AND_DIE("SEntityFactory::SpawnEntity - SystemContext does not have full ECS access, cannot spawn entity.");
    }

	ASSERT_OR_DIE(def != nullptr, "Null entity definition passed to SEntityFactory::CreateEntityFromDef");

    EntityID id = context.CreateEntity();
    if (id == EntityID::Invalid)
    {
        DevConsoleUtils::LogError("Max entities (%i) reached, cannot spawn entity from definition: %s", MAX_ENTITIES, def->m_name.ToCStr());
        return EntityID::Invalid;
    }

    // Add components that exist in the def
    if (def->m_transform.has_value())           context.AddComponent<CTransform>(id, *def->m_transform);
    if (def->m_time.has_value())                context.AddComponent<CTime>(id, *def->m_time);
    if (def->m_ai.has_value())                  context.AddComponent<CAIController>(id, *def->m_ai);
	if (def->m_animation.has_value())           context.AddComponent<CAnimation>(id, *def->m_animation);
	if (def->m_attachment.has_value())          context.AddComponent<CAttachment>(id, *def->m_attachment);
	if (def->m_collisionEffect.has_value())     context.AddComponent<CCollisionEffect>(id, *def->m_collisionEffect);
	if (def->m_collision.has_value())           context.AddComponent<CCollision>(id, *def->m_collision);
    if (def->m_movement.has_value())            context.AddComponent<CMovement>(id, *def->m_movement);
    if (def->m_render.has_value())              context.AddComponent<CRender>(id, *def->m_render);
	if (def->m_lifetime.has_value())            context.AddComponent<CLifetime>(id, *def->m_lifetime);
	if (def->m_health.has_value())              context.AddComponent<CHealth>(id, *def->m_health);
	if (def->m_death.has_value())               context.AddComponent<CDeath>(id, *def->m_death);
	if (def->m_ability.has_value())             context.AddComponent<CAbility>(id, *def->m_ability);
	if (def->m_proj.has_value())                context.AddComponent<CProjectile>(id, *def->m_proj);
	if (def->m_tags.has_value())                context.AddComponent<CTags>(id, *def->m_tags);

    // Todo: don't add this component, in release only?
    CEntityDebug* debugComponent = context.AddComponent<CEntityDebug>(id);
    debugComponent->m_defName = def->m_name;

    return id;
}



//----------------------------------------------------------------------------------------------------------------------
EntityID SEntityFactory::SpawnEntity(SystemContext const& context, SpawnInfo const& spawnInfo)
{
    if (!context.HasFullECSAccess())
    {
		ERROR_AND_DIE("SEntityFactory::SpawnEntity - SystemContext does not have full ECS access, cannot spawn entity.");
    }

    EntityID id = SEntityFactory::CreateEntityFromDef(context, spawnInfo.m_def);
    if (id == EntityID::Invalid)
    {
        return EntityID::Invalid;
    }

    if (CTransform* transform = context.GetComponent<CTransform>(id))
    {
        transform->m_pos = spawnInfo.m_spawnPos;
        transform->m_orientation = spawnInfo.m_spawnOrientation;
    }

    if (CCollision* collision = context.GetComponent<CCollision>(id))
    {
        collision->m_radius *= spawnInfo.m_spawnScale;
		collision->m_offset *= spawnInfo.m_spawnScale;
    }

    if (CRender* render = context.GetComponent<CRender>(id))
    {
        render->m_renderRadius *= spawnInfo.m_spawnScale;
    }

    if (CHealth* health = context.GetComponent<CHealth>(id))
    {
        health->m_maxHealth *= spawnInfo.m_spawnHealthMultiplier;
        health->m_currentHealth *= spawnInfo.m_spawnHealthMultiplier;
	}

    if (CMovement* movement = context.GetComponent<CMovement>(id))
    {
        movement->m_movementSpeedMultiplier = spawnInfo.m_spawnSpeedMultiplier;
	}

    CLifetime* lifetime = context.GetComponent<CLifetime>(id);
    if (!lifetime && spawnInfo.m_spawnLifetime >= 0.f)
    {
        lifetime = context.AddComponent<CLifetime>(id);
        lifetime->m_lifetime = spawnInfo.m_spawnLifetime;
        lifetime->m_lifetimeRemaining = spawnInfo.m_spawnLifetime;
    }

    return id;
}
