// Bradley Christensen - 2022-2026
#include "Ability.h"
#include "CProjectile.h"
#include "CTime.h"
#include "EntityDef.h"
#include "SCCollision.h"
#include "SCFlowField.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "AbilityDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Renderer/VertexUtils.h"
#include <set>



//----------------------------------------------------------------------------------------------------------------------
static constexpr float s_baseCritMultiplier = 2.f; // todo: move to gameplay static constants file?



//----------------------------------------------------------------------------------------------------------------------
AbilityTargetingComponent::AbilityTargetingComponent(AbilityTargetingComponentDef const& def)
{
    m_minRange = def.m_minRange;
	m_maxRange = def.m_maxRange;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityTargetingComponent::HasRangeChanged() const
{
	return m_minRange != m_minRangeAtTimeOfCache || m_maxRange != m_maxRangeAtTimeOfCache;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityTargetingComponent::AppendDebugString(std::string& out_string) const
{
    if (m_minRange > 0.f)
    {
        out_string += StringUtils::StringF("Range: %.1f - %.1f\n", m_minRange, m_maxRange);
    }
    else
    {
        out_string += StringUtils::StringF("Range: %.1f\n", m_maxRange);
    }
	out_string += StringUtils::StringF("Targeting Mode: %s\n", m_targetingMode == AbilityTargetingMode::ClosestToGoal ? "Closest To Goal" : "Unknown");
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCooldownComponent::AbilityCooldownComponent(AbilityCooldownComponentDef const& def)
{
	m_cooldownSeconds = def.m_cooldownSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Cooldown: %.1f\n", m_cooldownSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCritComponent::AbilityCritComponent(AbilityCritComponentDef const& def)
{
    m_critChance = def.m_critChance;
	m_critMulti = def.m_critMulti;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCritComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Crit Chance: %.1f%%\n", m_critChance * 100.f);
    out_string += StringUtils::StringF("Crit Mult: %.1f\n", 2.f + m_critMulti);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDamageComponent::AbilityDamageComponent(AbilityDamageComponentDef const& def)
{
    m_minDamage = def.m_minDamage;
	m_maxDamage = def.m_maxDamage;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityDamageComponent::AppendDebugString(std::string& out_string) const
{
    if (m_minDamage == m_maxDamage)
    {
        out_string += StringUtils::StringF("Damage: %.1f\n", m_minDamage);
    }
    else
    {
        out_string += StringUtils::StringF("Damage: %.1f - %.1f\n", m_minDamage, m_maxDamage);
    }
}



//----------------------------------------------------------------------------------------------------------------------
AbilityBurnComponent::AbilityBurnComponent(AbilityBurnComponentDef const& def)
{
	m_burn = def.m_burn;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityBurnComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Burn: %.1f\n", m_burn);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityPoisonComponent::AbilityPoisonComponent(AbilityPoisonComponentDef const& def)
{
	m_poison = def.m_poison;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityPoisonComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Poison: %.1f\n", m_poison);
}



//----------------------------------------------------------------------------------------------------------------------
AbilitySlowComponent::AbilitySlowComponent(AbilitySlowComponentDef const& def)
{
	m_duration = def.m_duration;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilitySlowComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Slow: %.1f\n", m_duration);
}




//----------------------------------------------------------------------------------------------------------------------
AbilityOnHitComponent::AbilityOnHitComponent(AbilityOnHitComponentDef const& def)
{
    m_damageOnHit = def.m_damageOnHit;
	m_poisonOnHit = def.m_poisonOnHit;
	m_burnOnHit = def.m_burnOnHit;
	m_aoeHitOnHit = def.m_aoeHitOnHit;
	m_aoeEffectOnHit = def.m_aoeEffectOnHit;
	m_slowOnHit = def.m_slowOnHit;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload AbilityOnHitComponent::GetPayload() const
{
	HitPayload payload;
	payload.m_damage = m_damageOnHit.has_value() ? m_damageOnHit->m_damageDone : 0.f;
	payload.m_burn = m_burnOnHit.has_value() ? m_burnOnHit->m_burn : 0.f;
	payload.m_poison = m_poisonOnHit.has_value() ? m_poisonOnHit->m_poison : 0.f;
	payload.m_slowDuration = m_slowOnHit.has_value() ? m_slowOnHit->m_duration : 0.f;
    return payload;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityOnHitComponent::AppendDebugString(std::string& out_string) const
{
    if (m_damageOnHit.has_value())
    {
		m_damageOnHit->AppendDebugString(out_string);
    }
    if (m_poisonOnHit.has_value())
    {
		m_poisonOnHit->AppendDebugString(out_string);
    }
    if (m_burnOnHit.has_value())
    {
		m_burnOnHit->AppendDebugString(out_string);
	}
    if (m_aoeHitOnHit.has_value())
    {
        m_aoeHitOnHit->AppendDebugString(out_string);
    }
    if (m_aoeEffectOnHit.has_value())
    {
        m_aoeEffectOnHit->AppendDebugString(out_string);
	}
    if (m_slowOnHit.has_value())
    {
        m_slowOnHit->AppendDebugString(out_string);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEHitComponent::AbilityAoEHitComponent(AbilityAoEHitComponentDef const& def)
{
    m_radius = def.m_radius;
    m_damageOnHit = def.m_damageOnHit;
    m_poisonOnHit = def.m_poisonOnHit;
	m_burnOnHit = def.m_burnOnHit;
	m_slowOnHit = def.m_slowOnHit;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload AbilityAoEHitComponent::GetPayload() const
{
    HitPayload payload;
    payload.m_damage = m_damageOnHit.has_value() ? m_damageOnHit->m_damageDone : 0.f;
    payload.m_burn = m_burnOnHit.has_value() ? m_burnOnHit->m_burn : 0.f;
    payload.m_poison = m_poisonOnHit.has_value() ? m_poisonOnHit->m_poison : 0.f;
    payload.m_slowDuration = m_slowOnHit.has_value() ? m_slowOnHit->m_duration : 0.f;
    return payload;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("AOE Hit Radius: %.1f\n", m_radius);
	out_string += "AoE Hit Components:\n";
    if (m_damageOnHit.has_value())
    {
        m_damageOnHit->AppendDebugString(out_string);
    }
    if (m_poisonOnHit.has_value())
    {
        m_poisonOnHit->AppendDebugString(out_string);
    }
    if (m_burnOnHit.has_value())
    {
        m_burnOnHit->AppendDebugString(out_string);
    }
    if (m_slowOnHit.has_value())
    {
        m_slowOnHit->AppendDebugString(out_string);
    }
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEEffectComponent::AbilityAoEEffectComponent(AbilityAoEEffectComponentDef const& def)
{
    m_aoeEffectDefName = def.m_aoeEffectDefName;
    m_radius = def.m_radius;
    m_durationSeconds = def.m_durationSeconds;
    m_damagePerSecond = def.m_damagePerSecond;
    m_poisonPerSecond = def.m_poisonPerSecond;
	m_burnPerSecond = def.m_burnPerSecond;
	m_slowPerSecond = def.m_slowPerSecond;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEEffectComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("AOE Effect Radius: %.1f\n", m_radius);
    out_string += StringUtils::StringF("AOE Effect Duration: %.1f\n", m_durationSeconds);
    out_string += "AoE Effect Components:\n";
    if (m_damagePerSecond.has_value())
    {
        m_damagePerSecond->AppendDebugString(out_string);
    }
    if (m_poisonPerSecond.has_value())
    {
        m_poisonPerSecond->AppendDebugString(out_string);
    }
    if (m_burnPerSecond.has_value())
    {
        m_burnPerSecond->AppendDebugString(out_string);
    }
}



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitAbility::ProjectileHitAbility(ProjectileHitAbilityDef const& def)
{
    m_abilityDef = &def;
    m_projectileDefName = def.m_projectileDefName;
    m_projSpeed = def.m_projSpeed;

    if (def.m_cooldownDef.has_value())
    {
        m_cooldownComp.emplace(def.m_cooldownDef.value());
    }
    if (def.m_targetingDef.has_value())
    {
        m_targetingComp.emplace(def.m_targetingDef.value());
    }
    if (def.m_critDef.has_value())
    {
        m_critComp.emplace(def.m_critDef.value());
    }
    if (def.m_onHitDef.has_value())
    {
        m_onHitComp.emplace(def.m_onHitDef.value());
    }
};



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::Update(float deltaSeconds, Vec2 const& location)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");
    ASSERT_OR_DIE(m_cooldownComp.has_value(), "ProjectileHitAbility::Update - m_cooldownComp is null.");
    ASSERT_OR_DIE(m_targetingComp.has_value(), "ProjectileHitAbility::Update - m_targetingComp is null.");
    ASSERT_OR_DIE(m_onHitComp.has_value(), "ProjectileHitAbility::Update - m_onHitComp is null.");

    m_cooldownComp->m_accumulatedTime += deltaSeconds;

    constexpr float maxAttacksPerSecond = 1000.f;
    constexpr float minTimeBetweenAttacks = 1.f / maxAttacksPerSecond;

    float timeBetweenAttacks = m_cooldownComp->m_cooldownSeconds;
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, minTimeBetweenAttacks);

    if (m_cooldownComp->m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

    // Find targets
    SCCollision const& collision = g_ecs->GetSingleton<SCCollision>();
    SCFlowField const& flowfield = g_ecs->GetSingleton<SCFlowField>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
    auto const& healthStorage = g_ecs->GetArrayStorage<CHealth>();

    CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);

    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
    bool hasRangeChanged = m_targetingComp->HasRangeChanged();
    if (hasRangeChanged)
    {
        world.ForEachPathTileInRange(location, m_targetingComp->m_minRange, m_targetingComp->m_maxRange, [&](IntVec2 const& worldCoords)
        {
            m_targetingComp->m_cachedPathTilesInRange.push_back(worldCoords);
            return true;
        });
        m_targetingComp->m_minRangeAtTimeOfCache = m_targetingComp->m_minRange;
        m_targetingComp->m_maxRangeAtTimeOfCache = m_targetingComp->m_maxRange;
    }

    static std::vector<EntityID> validTargets;
    validTargets.clear();

    EntityID targetID = EntityID::Invalid;
    if (m_targetingComp->m_targetingMode == AbilityTargetingMode::ClosestToGoal)
    {

        float closestToGoalDist = FLT_MAX;
        int closestToGoalTileIndex = -1;
        for (IntVec2 const& cachedPathTile : m_targetingComp->m_cachedPathTilesInRange)
        {
            int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);
            CollisionBucket const& tileBucket = enemyLayer[tileIndex];
            if (tileBucket.empty())
            {
                continue;
            }

            float distance = flowfield.m_toGoalFlowField.m_distanceField.Get(tileIndex);
            if (distance < closestToGoalDist)
            {
                bool tileHasValidTarget = false;

                for (EntityID entityID : tileBucket)
                {
                    CHealth const* healthComp = healthStorage.Get(entityID.GetIndex());
                    if (healthComp && healthComp->GetIsTargetable() && !healthComp->GetHealthReachedZero())
                    {
                        tileHasValidTarget = true;
                        break;
                    }
                }

                if (tileHasValidTarget)
                {
                    closestToGoalTileIndex = tileIndex;
                    closestToGoalDist = distance;
                }
            }
        }

        if (closestToGoalTileIndex != -1)
        {
            for (EntityID entityID : enemyLayer[closestToGoalTileIndex])
            {
                CHealth const* healthComp = healthStorage.Get(entityID.GetIndex());
                if (healthComp && healthComp->GetIsTargetable() && !healthComp->GetHealthReachedZero())
                {
                    validTargets.push_back(entityID);
                }
            }

            ASSERT_OR_DIE(!validTargets.empty(), "ProjectileHitAbility::Update - closest to goal tile has no valid targets. Should be impossible.");

            int randomIndex = g_rng->GetRandomIntInRange(0, static_cast<int>(validTargets.size()) - 1);
            targetID = validTargets[randomIndex];
        }
    }

    if (targetID == EntityID::Invalid)
    {
        // No targets in range, clamp cooldown
        m_cooldownComp->m_accumulatedTime = MathUtils::Clamp(m_cooldownComp->m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
	}

    // Shoot at targets
    while (m_cooldownComp->m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp->m_accumulatedTime -= timeBetweenAttacks;

        SpawnInfo spawnInfo;
        spawnInfo.m_spawnPos = location;
        spawnInfo.m_spawnOrientation = 0.f;
        spawnInfo.m_def = EntityDef::GetEntityDef(m_projectileDefName);
        EntityID projectileID = SEntityFactory::SpawnEntity(spawnInfo);
        if (!g_ecs->IsValid(projectileID))
        {
            break;
        }

        RollDamageAndEffects();

        // Copy ability data to proj, snapshotted with damage and effects already rolled.
        CProjectile& projComp = *g_ecs->GetComponent<CProjectile>(projectileID);
        projComp.m_targetID = targetID;
        projComp.m_targetPos = std::nullopt;
        projComp.m_accumulatedTime += m_cooldownComp->m_accumulatedTime;
        projComp.m_projSpeed = m_projSpeed;
        projComp.m_critComp = m_critComp;
        projComp.m_onHitComp = m_onHitComp;
    }
}



//----------------------------------------------------------------------------------------------------------------------
Ability* ProjectileHitAbility::DeepCopy() const
{
    ProjectileHitAbility* copy = new ProjectileHitAbility(*reinterpret_cast<ProjectileHitAbilityDef const*>(m_abilityDef));
    *copy = *this;
    return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
{
    if (m_targetingComp->m_minRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, m_targetingComp->m_minRange, 0.1f, 32, Rgba8::Green);
    }
    if (m_targetingComp->m_maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, m_targetingComp->m_maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Ability: %s\n", m_abilityDef->m_name.ToCStr());
    out_string += StringUtils::StringF("Proj Def: %s\n", m_projectileDefName.ToCStr());
    out_string += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed);

    if (m_cooldownComp.has_value())
    {
        m_cooldownComp->AppendDebugString(out_string);
    }
    if (m_targetingComp.has_value())
    {
        m_targetingComp->AppendDebugString(out_string);
    }
    if (m_critComp.has_value())
    {
        m_critComp->AppendDebugString(out_string);
    }
    if (m_onHitComp.has_value())
    {
        m_onHitComp->AppendDebugString(out_string);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::RollDamageAndEffects()
{
	float critMultiplier = s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.has_value())
    {
        AbilityCritComponent& critComp = m_critComp.value();
        float critRoll = g_rng->GetRandomFloatZeroToOne();
        critMultiplier += critComp.m_critMulti;
        critComp.m_didCrit = critRoll < critComp.m_critChance;
        didCrit = critComp.m_didCrit;
    }

    if (m_onHitComp.has_value())
    {
        AbilityOnHitComponent& onHitComp = m_onHitComp.value();
        if (onHitComp.m_damageOnHit.has_value())
        {
            AbilityDamageComponent& damageComp = onHitComp.m_damageOnHit.value();
            damageComp.m_damageDone = g_rng->GetRandomFloatInRange(damageComp.m_minDamage, damageComp.m_maxDamage);
            if (didCrit)
            {
                damageComp.m_damageDone *= critMultiplier;
            }
        }

        if (onHitComp.m_burnOnHit.has_value())
        {
            AbilityBurnComponent& burnComp = onHitComp.m_burnOnHit.value();
            burnComp.m_burnDone = burnComp.m_burn;
            if (didCrit)
            {
                burnComp.m_burnDone *= critMultiplier;
            }
        }

        if (onHitComp.m_poisonOnHit.has_value())
        {
            AbilityPoisonComponent& poisonComp = onHitComp.m_poisonOnHit.value();
            poisonComp.m_poisonDone = poisonComp.m_poison;
            if (didCrit)
            {
                poisonComp.m_poisonDone *= critMultiplier;
            }
        }

        if (onHitComp.m_aoeHitOnHit.has_value())
        {
            AbilityAoEHitComponent& aoeHitComp = onHitComp.m_aoeHitOnHit.value();
            if (aoeHitComp.m_damageOnHit.has_value())
            {
                AbilityDamageComponent& aoeDamageComp = aoeHitComp.m_damageOnHit.value();
                aoeDamageComp.m_damageDone = g_rng->GetRandomFloatInRange(aoeDamageComp.m_minDamage, aoeDamageComp.m_maxDamage);
                if (didCrit)
                {
                    aoeDamageComp.m_damageDone *= critMultiplier;
                }
            }

            if (aoeHitComp.m_burnOnHit.has_value())
            {
                AbilityBurnComponent& aoeBurnComp = aoeHitComp.m_burnOnHit.value();
                aoeBurnComp.m_burnDone = aoeBurnComp.m_burn;
                if (didCrit)
                {
                    aoeBurnComp.m_burnDone *= critMultiplier;
                }
            }

            if (aoeHitComp.m_poisonOnHit.has_value())
            {
                AbilityPoisonComponent& aoePoisonComp = aoeHitComp.m_poisonOnHit.value();
                aoePoisonComp.m_poisonDone = aoePoisonComp.m_poison;
                if (didCrit)
                {
                    aoePoisonComp.m_poisonDone *= critMultiplier;
                }
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
AoEHitAbility::AoEHitAbility(AoEHitAbilityDef const& def)
{
    m_abilityDef = &def;
    if (def.m_cooldownDef.has_value())
    {
        m_cooldownComp.emplace(def.m_cooldownDef.value());
    }
    if (def.m_targetingDef.has_value())
    {
        m_targetingComp.emplace(def.m_targetingDef.value());
    }
    if (def.m_critDef.has_value())
    {
        m_critComp.emplace(def.m_critDef.value());
    }
    if (def.m_aoeHitDef.has_value())
    {
        m_aoeHitComp.emplace(def.m_aoeHitDef.value());
	}
    if (def.m_aoeEffectDef.has_value())
    {
        m_aoeEffectComp.emplace(def.m_aoeEffectDef.value());
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::Update(float deltaSeconds, Vec2 const& location)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");
    ASSERT_OR_DIE(m_cooldownComp.has_value(), "ProjectileHitAbility::Update - m_cooldownComp is null.");
    ASSERT_OR_DIE(m_targetingComp.has_value(), "ProjectileHitAbility::Update - m_targetingComp is null.");
    ASSERT_OR_DIE(m_aoeHitComp.has_value(), "ProjectileHitAbility::Update - m_aoeHitComp is null.");

    m_cooldownComp->m_accumulatedTime += deltaSeconds;

    constexpr float maxAttacksPerSecond = 1000.f;
    constexpr float minTimeBetweenAttacks = 1.f / maxAttacksPerSecond;

    float timeBetweenAttacks = m_cooldownComp->m_cooldownSeconds;
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, minTimeBetweenAttacks);

    if (m_cooldownComp->m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

    // Find targets
    SCCollision const& collision = g_ecs->GetSingleton<SCCollision>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
    auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();

    CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);

    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
    bool hasRangeChanged = m_targetingComp->HasRangeChanged();
    if (hasRangeChanged)
    {
        world.ForEachPathTileInRange(location, m_targetingComp->m_minRange, m_targetingComp->m_maxRange, [&](IntVec2 const& worldCoords)
        {
            m_targetingComp->m_cachedPathTilesInRange.push_back(worldCoords);
            return true;
        });
        m_targetingComp->m_minRangeAtTimeOfCache = m_targetingComp->m_minRange;
        m_targetingComp->m_maxRangeAtTimeOfCache = m_targetingComp->m_maxRange;
    }

    static std::set<EntityID> validTargets;
    validTargets.clear();

    for (IntVec2 const& cachedPathTile : m_targetingComp->m_cachedPathTilesInRange)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);
        CollisionBucket const& tileBucket = enemyLayer[tileIndex];
        if (tileBucket.empty())
        {
            continue;
        }

        validTargets.insert(tileBucket.begin(), tileBucket.end());
    }

    if (validTargets.empty())
    {
        // No targets in range, clamp cooldown
        m_cooldownComp->m_accumulatedTime = MathUtils::Clamp(m_cooldownComp->m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

    while (m_cooldownComp->m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp->m_accumulatedTime -= timeBetweenAttacks;

		RollDamageAndEffects();

        if (m_aoeEffectComp.has_value())
        {
            SpawnInfo aoeEffectSpawnInfo;
            aoeEffectSpawnInfo.m_spawnPos = location;
            aoeEffectSpawnInfo.m_spawnLifetime = m_aoeEffectComp->m_durationSeconds;
            aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_aoeEffectComp->m_aoeEffectDefName);
            aoeEffectSpawnInfo.m_spawnScale = m_targetingComp->m_maxRange;

            EntityID aoeEffect = SEntityFactory::SpawnEntity(aoeEffectSpawnInfo);
            if (g_ecs->IsValid(aoeEffect))
            {
                // Pass along damage, color, to aoe effect
                if (CCollisionEffect* aoeEffectComp = g_ecs->GetComponent<CCollisionEffect>(aoeEffect))
                {
                    if (m_aoeEffectComp->m_damagePerSecond.has_value())
                    {
                        aoeEffectComp->m_damagePerSecond = m_aoeEffectComp->m_damagePerSecond->m_maxDamage;
                    }
                    if (m_aoeEffectComp->m_burnPerSecond.has_value())
                    {
                        aoeEffectComp->m_burnPerSecond = m_aoeEffectComp->m_burnPerSecond->m_burn;
                    }
                    if (m_aoeEffectComp->m_poisonPerSecond.has_value())
                    {
                        aoeEffectComp->m_poisonPerSecond = m_aoeEffectComp->m_poisonPerSecond->m_poison;
                    }
                    if (m_aoeEffectComp->m_slowPerSecond.has_value())
                    {
                        aoeEffectComp->m_slowPerSecond = m_aoeEffectComp->m_slowPerSecond->m_duration;
                    }
                }
            }
		}

        HitPayload payload;
		payload.m_burn = m_aoeHitComp->m_burnOnHit.has_value() ? m_aoeHitComp->m_burnOnHit->m_burn : 0.f;
		payload.m_damage = m_aoeHitComp->m_damageOnHit.has_value() ? m_aoeHitComp->m_damageOnHit->m_damageDone : 0.f;
		payload.m_poison = m_aoeHitComp->m_poisonOnHit.has_value() ? m_aoeHitComp->m_poisonOnHit->m_poison : 0.f;
		payload.m_slowDuration = m_aoeHitComp->m_slowOnHit.has_value() ? m_aoeHitComp->m_slowOnHit->m_duration : 0.f;

        for (EntityID entityID : validTargets)
        {
            if (payload.IsRelevantToHealth())
            {
                CHealth* healthComp = healthStorage.Get(entityID.GetIndex());
                if (healthComp)
                {
                    healthComp->TakePayload(payload);
                }
            }

            if (payload.IsRelevantToTime())
            {
                CTime* timeComp = g_ecs->GetComponent<CTime>(entityID);
				timeComp->m_remainingSlowDuration += payload.m_slowDuration;
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AoEHitAbility::DeepCopy() const
{
    AoEHitAbility* copy = new AoEHitAbility(*reinterpret_cast<AoEHitAbilityDef const*>(m_abilityDef));
    *copy = *this;
    return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
{
    if (m_targetingComp->m_minRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, m_targetingComp->m_minRange, 0.1f, 32, Rgba8::Green);
    }
    if (m_targetingComp->m_maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, m_targetingComp->m_maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Ability: %s\n", m_abilityDef->m_name.ToCStr());

    if (m_cooldownComp.has_value())
    {
        m_cooldownComp->AppendDebugString(out_string);
    }
    if (m_targetingComp.has_value())
    {
        m_targetingComp->AppendDebugString(out_string);
    }
    if (m_critComp.has_value())
    {
        m_critComp->AppendDebugString(out_string);
    }
    if (m_aoeHitComp.has_value())
    {
        m_aoeHitComp->AppendDebugString(out_string);
    }
    if (m_aoeEffectComp.has_value())
    {
        m_aoeEffectComp->AppendDebugString(out_string);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::RollDamageAndEffects()
{
    float critMultiplier = s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.has_value())
    {
        AbilityCritComponent& critComp = m_critComp.value();
        float critRoll = g_rng->GetRandomFloatZeroToOne();
        critMultiplier += critComp.m_critMulti;
        critComp.m_didCrit = critRoll < critComp.m_critChance;
        didCrit = critComp.m_didCrit;
    }

    if (m_aoeHitComp.has_value())
    {
        AbilityAoEHitComponent& aoeHitComp = m_aoeHitComp.value();
        if (aoeHitComp.m_damageOnHit.has_value())
        {
            AbilityDamageComponent& damageComp = aoeHitComp.m_damageOnHit.value();
            damageComp.m_damageDone = g_rng->GetRandomFloatInRange(damageComp.m_minDamage, damageComp.m_maxDamage);
            if (didCrit)
            {
                damageComp.m_damageDone *= critMultiplier;
            }
        }

        if (aoeHitComp.m_burnOnHit.has_value())
        {
            AbilityBurnComponent& burnComp = aoeHitComp.m_burnOnHit.value();
            burnComp.m_burnDone = burnComp.m_burn;
            if (didCrit)
            {
                burnComp.m_burnDone *= critMultiplier;
            }
        }

        if (aoeHitComp.m_poisonOnHit.has_value())
        {
            AbilityPoisonComponent& poisonComp = aoeHitComp.m_poisonOnHit.value();
            poisonComp.m_poisonDone = poisonComp.m_poison;
            if (didCrit)
            {
                poisonComp.m_poisonDone *= critMultiplier;
            }
        }
    }
}
