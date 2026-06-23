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
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"
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
bool AbilityTargetingComponent::NeedsCacheUpdate(Vec2 const& location) const
{
	return m_minRange != m_minRangeAtTimeOfCache || m_maxRange != m_maxRangeAtTimeOfCache || m_locationAtTimeOfCache != location;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityTargetingComponent::UpdateCachedTiles(SystemContext const& context, Vec2 const& location)
{
    bool needsUpdate = NeedsCacheUpdate(location);
    if (needsUpdate)
    {
        SCWorld const& world = context.GetSingletonConst<SCWorld>();

        world.ForEachPathTileInRange(location, m_minRange, m_maxRange, [&](IntVec2 const& worldCoords)
        {
            m_cachedPathTilesInRange.push_back(worldCoords);
            return true;
        });

        m_minRangeAtTimeOfCache = m_minRange;
        m_maxRangeAtTimeOfCache = m_maxRange;
        m_locationAtTimeOfCache = location;

        if (m_targetingMode == AbilityTargetingMode::ClosestToGoal)
        {
			SCFlowField const& flowfield = context.GetSingletonConst<SCFlowField>();
            std::sort(m_cachedPathTilesInRange.begin(), m_cachedPathTilesInRange.end(), [&](IntVec2 const& a, IntVec2 const& b)
            {
                int aTileIndex = world.m_tiles.GetIndexForCoords(a);
                int bTileIndex = world.m_tiles.GetIndexForCoords(b);
                return flowfield.m_toGoalFlowField.m_distanceField.Get(aTileIndex) < flowfield.m_toGoalFlowField.m_distanceField.Get(bTileIndex);
            });
		}
    }
    return needsUpdate;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityTargetingComponent::FindTargets(SystemContext const& context, int maxTargets /*= -1*/)
{
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	SCFlowField const& flowfield = context.GetSingletonConst<SCFlowField>();
	SCCollision const& collision = context.GetSingletonConst<SCCollision>();
    CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);
	auto const& healthStorage = context.GetArrayStorageConst<CHealth>();

	BitMask healthBit = context.GetComponentBitMask<CHealth>();

    m_targets.clear();

    if (m_targetingMode == AbilityTargetingMode::ClosestToGoal)
    {
        for (IntVec2 const& cachedPathTile : m_cachedPathTilesInRange)
        {
            int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);
            CollisionBucket const& tileBucket = enemyLayer[tileIndex];
            if (tileBucket.empty())
            {
                continue;
            }

            for (EntityID entityID : tileBucket)
            {
                if (context.HasComponents(entityID, healthBit))
                {
                    CHealth const& healthComp = healthStorage[entityID];
                    if (healthComp.GetIsTargetable() && !healthComp.GetHealthReachedZero())
                    {
                        m_targets.push_back(entityID);

                        if (m_targets.size() >= (size_t) maxTargets && maxTargets > 0)
                        {
                            return true;
						}
                    }
                }
			}
        }
    }

    if (m_targetingMode == AbilityTargetingMode::AllInRange)
    {
        for (IntVec2 const& cachedPathTile : m_cachedPathTilesInRange)
        {
            int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);
            CollisionBucket const& tileBucket = enemyLayer[tileIndex];

            for (EntityID entityID : tileBucket)
            {
                if (context.HasComponents(entityID, healthBit))
                {
					CHealth const& healthComp = healthStorage[entityID];
                    if (healthComp.GetIsTargetable() && !healthComp.GetHealthReachedZero())
                    {
                        m_targets.push_back(entityID);

                        // Ignore maxTargets for AllInRange mode
                    }
                }
            }
        }
	}

	return !m_targets.empty();
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AbilityTargetingComponent::FindChainTarget(SystemContext const& context, Vec2 const& pos, float maxDistance)
{
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	SCCollision const& collision = context.GetSingletonConst<SCCollision>();

	CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);

	float maxDistanceSquared = maxDistance * maxDistance;

    world.ForEachPathTileInRange(pos, 0.f, maxDistance, [&](IntVec2 const& worldCoords)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
        CollisionBucket const& tileBucket = enemyLayer[tileIndex];

        for (EntityID entityID : tileBucket)
        {
            bool sameEntity = false;
            for (EntityID targetID : m_targets)
            {
                if (entityID == targetID)
                {
                    sameEntity = true;
                    break;
                }
			}

            if (sameEntity)
            {
                continue;
			}

			CTransform const& transformComp = *context.GetComponentConst<CTransform>(entityID);
			float distSquared = MathUtils::GetDistanceSquared2D(pos, transformComp.m_pos);
            if (distSquared > maxDistanceSquared)
            {
                continue;
			}

            CHealth const& healthComp = *context.GetComponentConst<CHealth>(entityID);
            if (healthComp.GetIsTargetable() && !healthComp.GetHealthReachedZero())
            {
                m_targets.push_back(entityID);
                return false;
            }
        }

        return true;
	});

    return EntityID::Invalid;
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
AbilityChainComponent::AbilityChainComponent(AbilityChainComponentDef const& def)
{
    m_chainChance = def.m_chainChance;
    m_chainDistance = def.m_chainDistance;
	m_chainPayloadMulti = def.m_chainPayloadMulti;
    m_maxChains = def.m_maxChains;
	m_remainingChains = def.m_maxChains;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityChainComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Chain Chance: %.1f%%\n", m_chainChance * 100.f);
    out_string += StringUtils::StringF("Max Chains: %d\n", m_maxChains);
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
HitPayload AbilityOnHitComponent::GetDoTPayload(float deltaSeconds) const
{
    HitPayload payload = GetPayload();
	payload.m_burn *= deltaSeconds;
	payload.m_poison *= deltaSeconds;
	payload.m_damage *= deltaSeconds;
	payload.m_slowDuration *= deltaSeconds;
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
void Ability::RollDamageAndEffects(RandomNumberGenerator&)
{
    // Empty
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
	m_renderComp = def.m_renderDef;
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
Ability::Ability(AbilityDef const& def) : m_abilityDef(&def)
{
}



//----------------------------------------------------------------------------------------------------------------------
void Ability::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Ability: %s\n", m_abilityDef ? m_abilityDef->m_name.ToCStr() : "Invalid");
}



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitAbility::ProjectileHitAbility(ProjectileHitAbilityDef const& def) : Ability(def)
{
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
    if (def.m_chainDef.has_value())
    {
        m_chainComp.emplace(def.m_chainDef.value());
	}
};



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::Update(SystemContext const& context, Vec2 const& location)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");
    ASSERT_OR_DIE(m_cooldownComp.has_value(), "ProjectileHitAbility::Update - m_cooldownComp is null.");
    ASSERT_OR_DIE(m_targetingComp.has_value(), "ProjectileHitAbility::Update - m_targetingComp is null.");
    ASSERT_OR_DIE(m_onHitComp.has_value(), "ProjectileHitAbility::Update - m_onHitComp is null.");

    float deltaSeconds = context.m_deltaSeconds;
    m_cooldownComp->m_accumulatedTime += deltaSeconds;

    constexpr float maxAttacksPerSecond = 1000.f;
    constexpr float minTimeBetweenAttacks = 1.f / maxAttacksPerSecond;

    float timeBetweenAttacks = m_cooldownComp->m_cooldownSeconds;
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, minTimeBetweenAttacks);

    if (m_cooldownComp->m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

	// Write Dependencies
	auto& projectileStorage = context.GetMapStorage<CProjectile>();
    RandomNumberGenerator& rng = *context.GetSingleton<SCRandomNumberGenerator>().GetRNG();

    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
    m_targetingComp->UpdateCachedTiles(context, location);

    if (!m_targetingComp->FindTargets(context, 1))
    {
        // No targets in range, clamp cooldown
        m_cooldownComp->m_accumulatedTime = MathUtils::Clamp(m_cooldownComp->m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

	EntityID targetID = m_targetingComp->m_targets.front(); // todo: support multiple targets

    // Shoot at targets
    while (m_cooldownComp->m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp->m_accumulatedTime -= timeBetweenAttacks;

        SpawnInfo spawnInfo;
        spawnInfo.m_spawnPos = location;
        spawnInfo.m_spawnOrientation = 0.f;
        spawnInfo.m_def = EntityDef::GetEntityDef(m_projectileDefName);
        EntityID projectileID = SEntityFactory::SpawnEntity(context, spawnInfo);
        if (!context.IsValid(projectileID))
        {
            break;
        }

        RollDamageAndEffects(rng);

        // Copy ability data to proj, snapshotted with damage and effects already rolled.
		ASSERT_OR_DIE(context.HasComponent<CProjectile>(projectileID), "ProjectileHitAbility::Update - spawned projectile is missing CProjectile component.");
		CProjectile& projComp = projectileStorage[projectileID];

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
    if (m_targetingComp.has_value())
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
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::AppendDebugString(std::string& out_string) const
{
	Ability::AppendDebugString(out_string);
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
void ProjectileHitAbility::RollDamageAndEffects(RandomNumberGenerator& rng)
{
	float critMultiplier = s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.has_value())
    {
        AbilityCritComponent& critComp = m_critComp.value();
        float critRoll = rng.GetRandomFloatZeroToOne();
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
            damageComp.m_damageDone = rng.GetRandomFloatInRange(damageComp.m_minDamage, damageComp.m_maxDamage);
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
                aoeDamageComp.m_damageDone = rng.GetRandomFloatInRange(aoeDamageComp.m_minDamage, aoeDamageComp.m_maxDamage);
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
AoEHitAbility::AoEHitAbility(AoEHitAbilityDef const& def) : Ability(def)
{
    if (def.m_cooldownDef.has_value())
    {
        m_cooldownComp.emplace(def.m_cooldownDef.value());
    }
    if (def.m_targetingDef.has_value())
    {
        m_targetingComp.emplace(def.m_targetingDef.value());
		m_targetingComp->m_targetingMode = AbilityTargetingMode::AllInRange; // All In Range is the only targeting mode that makes sense for an AoE hit
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
void AoEHitAbility::Update(SystemContext const& context, Vec2 const& location)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");
    ASSERT_OR_DIE(m_cooldownComp.has_value(), "ProjectileHitAbility::Update - m_cooldownComp is null.");
    ASSERT_OR_DIE(m_targetingComp.has_value(), "ProjectileHitAbility::Update - m_targetingComp is null.");
    ASSERT_OR_DIE(m_aoeHitComp.has_value(), "ProjectileHitAbility::Update - m_aoeHitComp is null.");

	float deltaSeconds = context.m_deltaSeconds;
    m_cooldownComp->m_accumulatedTime += deltaSeconds;

    constexpr float maxAttacksPerSecond = 1000.f;
    constexpr float minTimeBetweenAttacks = 1.f / maxAttacksPerSecond;

    float timeBetweenAttacks = m_cooldownComp->m_cooldownSeconds;
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, minTimeBetweenAttacks);

    if (m_cooldownComp->m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

	// Write Dependencies
    auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();
	auto& collisionEffectStorage = context.GetArrayStorage<CCollisionEffect>();
    RandomNumberGenerator& rng = *context.GetSingleton<SCRandomNumberGenerator>().GetRNG();
	// CAbility (bc this is an ability in a CAbility that can update itself)
	// Spawn Entities (All)

	BitMask healthBit = context.GetComponentBitMask<CHealth>();
	BitMask timeBit = context.GetComponentBitMask<CTime>();
	BitMask collisionEffectBit = context.GetComponentBitMask<CCollisionEffect>();

    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
	m_targetingComp->UpdateCachedTiles(context, location);

	if (!m_targetingComp->FindTargets(context))
    {
        // No targets in range, clamp cooldown
        m_cooldownComp->m_accumulatedTime = MathUtils::Clamp(m_cooldownComp->m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

    while (m_cooldownComp->m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp->m_accumulatedTime -= timeBetweenAttacks;

		RollDamageAndEffects(rng);

        if (m_aoeEffectComp.has_value())
        {
            SpawnInfo aoeEffectSpawnInfo;
            aoeEffectSpawnInfo.m_spawnPos = location;
            aoeEffectSpawnInfo.m_spawnLifetime = m_aoeEffectComp->m_durationSeconds;
            aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_aoeEffectComp->m_aoeEffectDefName);
            aoeEffectSpawnInfo.m_spawnScale = m_targetingComp->m_maxRange; // Initial radius is assumed to be 1

            EntityID aoeEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);

            // Pass along damage, color, to aoe effect
            if (context.HasComponents(aoeEffect, collisionEffectBit))
            {
                CCollisionEffect& aoeEffectComp = collisionEffectStorage[aoeEffect];
                aoeEffectComp.InitializeFromAoEEffect(m_aoeEffectComp.value());
            }
		}

        HitPayload payload;
		payload.m_burn = m_aoeHitComp->m_burnOnHit.has_value() ? m_aoeHitComp->m_burnOnHit->m_burn : 0.f;
		payload.m_damage = m_aoeHitComp->m_damageOnHit.has_value() ? m_aoeHitComp->m_damageOnHit->m_damageDone : 0.f;
		payload.m_poison = m_aoeHitComp->m_poisonOnHit.has_value() ? m_aoeHitComp->m_poisonOnHit->m_poison : 0.f;
		payload.m_slowDuration = m_aoeHitComp->m_slowOnHit.has_value() ? m_aoeHitComp->m_slowOnHit->m_duration : 0.f;

        if (!payload.HasValue())
        {
            continue;
        }

        for (EntityID entityID : m_targetingComp->m_targets)
        {
            if (payload.IsRelevantToHealth() && context.HasComponents(entityID, healthBit))
            {
                CHealth& healthComp = healthStorage[entityID];
                healthComp.TakePayload(payload);
            }

            if (payload.IsRelevantToTime() && context.HasComponents(entityID, timeBit))
            {
				CTime& timeComp = timeStorage[entityID];
                timeComp.m_remainingSlowDuration += payload.m_slowDuration;
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
    if (m_targetingComp.has_value())
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
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::AppendDebugString(std::string& out_string) const
{
    Ability::AppendDebugString(out_string);

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
void AoEHitAbility::RollDamageAndEffects(RandomNumberGenerator& rng)
{
    float critMultiplier = s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.has_value())
    {
        AbilityCritComponent& critComp = m_critComp.value();
        float critRoll = rng.GetRandomFloatZeroToOne();
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
            damageComp.m_damageDone = rng.GetRandomFloatInRange(damageComp.m_minDamage, damageComp.m_maxDamage);
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



//----------------------------------------------------------------------------------------------------------------------
PassiveAoEAbility::PassiveAoEAbility(PassiveAoEAbilityDef const& def) : Ability(def)
{
    if (def.m_targetingDef.has_value())
    {
        m_targetingComp.emplace(def.m_targetingDef.value());
    }
    if (def.m_aoeEffectDef.has_value())
    {
        m_aoeEffectComp.emplace(def.m_aoeEffectDef.value());
	}
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::Update(SystemContext const& context, Vec2 const& location)
{
    ASSERT_OR_DIE(m_abilityDef, "PassiveAoEAbility::Update - m_abilityDef is null.");
    ASSERT_OR_DIE(m_targetingComp.has_value(), "PassiveAoEAbility::Update - m_targetingComp is null.");
    ASSERT_OR_DIE(m_aoeEffectComp.has_value(), "PassiveAoEAbility::Update - m_aoeEffectComp is null.");

    // Write Dependencies
	auto& collisionEffectStorage = context.GetArrayStorage<CCollisionEffect>();

	BitMask collisionEffectBit = context.GetComponentBitMask<CCollisionEffect>();

    if (m_activeAoEEffect == EntityID::Invalid)
    {
        SpawnInfo aoeEffectSpawnInfo;
        aoeEffectSpawnInfo.m_spawnPos = location;
        aoeEffectSpawnInfo.m_spawnLifetime = -1.f; // Infinite bc this is a passive ability
        aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_aoeEffectComp->m_aoeEffectDefName);
        aoeEffectSpawnInfo.m_spawnScale = m_targetingComp->m_maxRange;

        m_activeAoEEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);

        if (context.IsValid(m_activeAoEEffect))
        {
            // Pass along damage, color, to aoe effect
            if (context.HasComponents(m_activeAoEEffect, collisionEffectBit))
            {
				CCollisionEffect& aoeEffectComp = collisionEffectStorage[m_activeAoEEffect];
				aoeEffectComp.InitializeFromAoEEffect(m_aoeEffectComp.value());
            }
		}
    }
}



//----------------------------------------------------------------------------------------------------------------------
Ability* PassiveAoEAbility::DeepCopy() const
{
    PassiveAoEAbility* copy = new PassiveAoEAbility(*reinterpret_cast<PassiveAoEAbilityDef const*>(m_abilityDef));
    *copy = *this;
	return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
{
    if (m_targetingComp.has_value())
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
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::AppendDebugString(std::string& out_string) const
{
    Ability::AppendDebugString(out_string);

    if (m_aoeEffectComp.has_value())
    {
        m_aoeEffectComp->AppendDebugString(out_string);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityRenderComponent::AbilityRenderComponent(AbilityRenderComponentDef const& def)
{
    m_tint = def.m_tint;
	m_depth = def.m_depth;
}



//----------------------------------------------------------------------------------------------------------------------
LaserAbility::LaserAbility(LaserAbilityDef const& def) : Ability(def)
{
    if (def.m_targetingDef.has_value())
    {
        m_targetingComp.emplace(def.m_targetingDef.value());
    }
    if (def.m_onHitDef.has_value())
    {
        m_onHitComp.emplace(def.m_onHitDef.value());
	}
    if (def.m_renderDef.has_value())
    {
        m_renderComp.emplace(def.m_renderDef.value());
	}
    if (def.m_chainDef.has_value())
    {
        m_chainComp.emplace(def.m_chainDef.value());
    }
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Update(SystemContext const& context, Vec2 const& location)
{
	ASSERT_OR_DIE(m_abilityDef, "LaserAbility::Update - m_abilityDef is null.");
	ASSERT_OR_DIE(m_targetingComp.has_value(), "LaserAbility::Update - m_targetingComp is null.");
	ASSERT_OR_DIE(m_onHitComp.has_value(), "LaserAbility::Update - m_onHitComp is null.");

    if (context.m_deltaSeconds == 0.f)
    {
        return;
    }

    BitMask healthBit = context.GetComponentBitMask<CHealth>();
    BitMask timeBit = context.GetComponentBitMask<CTime>();

    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
    m_targetingComp->UpdateCachedTiles(context, location);

    if (!m_targetingComp->FindTargets(context, 1))
    {
        return;
    }

    if (m_chainComp.has_value())
    {
		m_chainComp->m_remainingChains = m_chainComp->m_maxChains;

        for (int i = 0; i < m_chainComp->m_maxChains; ++i)
        {
			EntityID currentTarget = m_targetingComp->m_targets.back();
			CTransform const& currentTargetTransform = *context.GetComponentConst<CTransform>(currentTarget);

            EntityID chainEntity = m_targetingComp->FindChainTarget(context, currentTargetTransform.m_pos, m_chainComp->m_chainDistance);
            if (chainEntity == EntityID::Invalid)
            {
                break;
			}
        }
    }

	float chainPayloadMulti = 1.f;

    for (EntityID target : m_targetingComp->m_targets)
    {
        HitPayload payload = m_onHitComp->GetDoTPayload(context.m_deltaSeconds);

		payload *= chainPayloadMulti;

        if (m_chainComp.has_value())
        {
            chainPayloadMulti *= m_chainComp->m_chainPayloadMulti;
		}

        if (payload.IsRelevantToHealth() && context.HasComponentsUnsafe(target.GetIndex(), healthBit))
        {
            CHealth& healthComp = context.GetArrayStorage<CHealth>()[target];
            healthComp.TakePayload(payload);
        }

        if (payload.IsRelevantToTime() && context.HasComponentsUnsafe(target.GetIndex(), timeBit))
        {
            CTime& timeComp = context.GetArrayStorage<CTime>()[target];
            timeComp.m_remainingSlowDuration += payload.m_slowDuration;
        }

        if (m_onHitComp->m_aoeEffectOnHit.has_value())
        {
            CTransform const& targetTransform = *context.GetComponentConst<CTransform>(target);

            SpawnInfo aoeEffectSpawnInfo;
            aoeEffectSpawnInfo.m_spawnPos = targetTransform.m_pos;
            aoeEffectSpawnInfo.m_spawnLifetime = m_onHitComp->m_aoeEffectOnHit->m_durationSeconds;
            aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_onHitComp->m_aoeEffectOnHit->m_aoeEffectDefName);
            aoeEffectSpawnInfo.m_spawnScale = m_onHitComp->m_aoeEffectOnHit->m_radius;

            EntityID aoeEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);
            if (context.IsValid(aoeEffect))
            {
                CCollisionEffect& aoeEffectComp = *context.GetComponent<CCollisionEffect>(aoeEffect);
                aoeEffectComp.InitializeFromAoEEffect(m_onHitComp->m_aoeEffectOnHit.value());
            }
        }
	}
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Render(SystemContext const& context, Vec2 const& location) const
{
	ASSERT_OR_DIE(m_renderComp.has_value(), "LaserAbility::Render - m_renderComp is null.");

	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    VertexBuffer& vbo = *renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
    vbo.ClearVerts();

    Vec2 currentChainStartLocation = location;

    for (EntityID target : m_targetingComp->m_targets)
    {
        if (CTransform const* transform = context.GetComponent<CTransform>(target))
        {
            VertexUtils::AddVertsForLine2D(vbo, currentChainStartLocation, transform->m_pos, 0.25f, m_renderComp->m_tint, m_renderComp->m_depth);
            currentChainStartLocation = transform->m_pos;
        }
    }


    renderer.BindShader();
    renderer.BindTexture();
    renderer.DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
Ability* LaserAbility::DeepCopy() const
{
    LaserAbility* copy = new LaserAbility(*reinterpret_cast<LaserAbilityDef const*>(m_abilityDef));
    *copy = *this;
	return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
{
    if (m_targetingComp.has_value())
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
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::AppendDebugString(std::string& out_string) const
{
    Ability::AppendDebugString(out_string);

    if (m_targetingComp.has_value())
    {
        m_targetingComp->AppendDebugString(out_string);
	}
    if (m_onHitComp.has_value())
    {
        m_onHitComp->AppendDebugString(out_string);
    }
}
