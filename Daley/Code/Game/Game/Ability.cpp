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
#include "Engine/Math/Grid.h"
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
            // Sort by distance so that when we go to find targets, the best target is first in the list.
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
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoETargetingComponent::AbilityAoETargetingComponent(AbilityTargetingComponentDef const& def) : AbilityTargetingComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoETargetingComponent::FindTargets(SystemContext const& context, int maxTargets /*= -1*/)
{
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	SCCollision const& collision = context.GetSingletonConst<SCCollision>();
	CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	BitMask healthBit = context.GetComponentBitMask<CHealth>();

    m_targets.clear();

    for (IntVec2 const& cachedPathTile : m_cachedPathTilesInRange)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);
        CollisionBucket const& tileBucket = enemyLayer[tileIndex];

        for (EntityID entityID : tileBucket)
        {
            if (m_targets.find(entityID) != m_targets.end())
            {
                continue;
            }

            if (context.HasComponents(entityID, healthBit))
            {
                CHealth const& healthComp = healthStorage[entityID];
                if (healthComp.GetIsTargetable() && !healthComp.GetHealthReachedZero())
                {
                    m_targets.insert(entityID);

                    if (maxTargets > 0 && m_targets.size() >= maxTargets)
                    {
                        return true;
                    }
                }
            }
        }
    }

	return !m_targets.empty();
}



//----------------------------------------------------------------------------------------------------------------------
AbilityPrecisionTargetingComponent::AbilityPrecisionTargetingComponent(AbilityTargetingComponentDef const& def) : AbilityTargetingComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityPrecisionTargetingComponent::FindTargets(SystemContext const& context, int maxTargets /*= 1*/, int maxChains /*= 0*/, float maxChainDistance /*= 1.f*/)
{
    if (maxTargets <= 0)
    {
        return false;
	}

    SCWorld const& world = context.GetSingletonConst<SCWorld>();
    SCCollision const& collision = context.GetSingletonConst<SCCollision>();
    CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);
    auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	auto& transStorage = context.GetArrayStorageConst<CTransform>();
    BitMask healthBit = context.GetComponentBitMask<CHealth>();

    IntVec2 targetChainDims = IntVec2(maxTargets, 1 + maxChains);
	m_targetChains.Initialize(targetChainDims, EntityID::Invalid);

    int numTargets = 0;

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
                if (m_targetChains.Contains(entityID))
                {
                    continue;
				}

                if (context.HasComponents(entityID, healthBit))
                {
                    CHealth const& healthComp = healthStorage[entityID];
                    if (healthComp.GetIsTargetable() && !healthComp.GetHealthReachedZero())
                    {
						IntVec2 targetCoords = IntVec2(numTargets, 0);
                        m_targetChains.Set(targetCoords, entityID);

						Vec2 currentChainPosition = transStorage[entityID].m_pos;

                        for (int chainIndex = 0; chainIndex < maxChains; ++chainIndex)
                        {
                            IntVec2 chainTargetCoords = IntVec2(numTargets, chainIndex + 1);
                            EntityID chainTarget = FindChainTarget(context, currentChainPosition, maxChainDistance);
                            if (chainTarget == EntityID::Invalid)
                            {
                                break;
                            }
                            m_targetChains.Set(chainTargetCoords, chainTarget);
                            currentChainPosition = transStorage[chainTarget].m_pos;
                        }

                        numTargets++;

                        if (numTargets >= maxTargets)
                        {
                            return true;
						}
                    }
                }
            }
        }
    }

    return numTargets > 0;
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AbilityPrecisionTargetingComponent::FindChainTarget(SystemContext const& context, Vec2 const& pos, float maxDistance)
{
    SCWorld const& world = context.GetSingletonConst<SCWorld>();
    SCCollision const& collision = context.GetSingletonConst<SCCollision>();

    CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);

    float maxDistanceSquared = maxDistance * maxDistance;

    EntityID result = EntityID::Invalid;

    world.ForEachPathTileInRange(pos, 0.f, maxDistance, [&](IntVec2 const& worldCoords)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
        CollisionBucket const& tileBucket = enemyLayer[tileIndex];

        for (EntityID entityID : tileBucket)
        {
            if (m_targetChains.Contains(entityID))
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
                result = entityID;
                return false;
            }
        }

        return true;
    });

    return result;
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
    if (m_critChance <= 0.f)
    {
        return;
	}
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
    if (m_minDamage <= 0.f && m_maxDamage <= 0.f)
    {
        return;
    }

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
    if (m_burn <= 0.f)
    {
        return;
	}
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
    if (m_poison <= 0.f)
    {
        return;
    }
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
    if (m_duration <= 0.f)
    {
        return;
	}
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
    out_string += StringUtils::StringF("Chain Chance: %.2f\n", m_chainChance * 100.f);
    out_string += StringUtils::StringF("Max Chains: %d\n", m_maxChains);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityMultishotComponent::AbilityMultishotComponent(AbilityMultishotComponentDef const& def)
{
	m_additionalTargets = def.m_additionalTargets;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityMultishotComponent::AppendDebugString(std::string& out_string) const
{
    if (m_additionalTargets <= 0)
    {
        return;
    }

	out_string += StringUtils::StringF("Additional Targets: %d\n", m_additionalTargets);
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
void AbilityOnHitComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("---Hit---\n");

	float minDamage = m_damageOnHit.has_value() ? m_damageOnHit->m_minDamage : 0.f;
	float maxDamage = m_damageOnHit.has_value() ? m_damageOnHit->m_maxDamage : 0.f;
	float burn = m_burnOnHit.has_value() ? m_burnOnHit->m_burn : 0.f;
	float poison = m_poisonOnHit.has_value() ? m_poisonOnHit->m_poison : 0.f;
	float slow = m_slowOnHit.has_value() ? m_slowOnHit->m_duration : 0.f;
	out_string += StringUtils::StringF("D(%.1f-%.1f) P(%.1f) B(%.1f) S(%.1f)\n", minDamage, maxDamage, poison, burn, slow);

    if (m_aoeHitOnHit.has_value())
    {
        m_aoeHitOnHit->AppendDebugString(out_string);
    }
    if (m_aoeEffectOnHit.has_value())
    {
        m_aoeEffectOnHit->AppendDebugString(out_string);
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
void AbilityAoEHitComponent::AppendDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("---AOE Hit---\n");
    if (m_radius > 0.f)
    {
        out_string += StringUtils::StringF("Radius: %.1f\n", m_radius);
    }	
    float minDamage = m_damageOnHit.has_value() ? m_damageOnHit->m_minDamage : 0.f;
    float maxDamage = m_damageOnHit.has_value() ? m_damageOnHit->m_maxDamage : 0.f;
    float burn = m_burnOnHit.has_value() ? m_burnOnHit->m_burn : 0.f;
    float poison = m_poisonOnHit.has_value() ? m_poisonOnHit->m_poison : 0.f;
    float slow = m_slowOnHit.has_value() ? m_slowOnHit->m_duration : 0.f;
    out_string += StringUtils::StringF("D(%.1f-%.1f) P(%.1f) B(%.1f) S(%.1f)\n", minDamage, maxDamage, poison, burn, slow);
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
    out_string += StringUtils::StringF("---AOE Effect---\n", m_radius);
    if (m_radius > 0.f)
    {
        out_string += StringUtils::StringF("Radius: %.1f\n", m_radius);
	}
    out_string += StringUtils::StringF("Duration: %.1f\n", m_durationSeconds);	

    float damage = m_damagePerSecond.has_value() ? m_damagePerSecond->m_maxDamage : 0.f;
    float burn = m_burnPerSecond.has_value() ? m_burnPerSecond->m_burn : 0.f;
    float poison = m_poisonPerSecond.has_value() ? m_poisonPerSecond->m_poison : 0.f;
    float slow = m_slowPerSecond.has_value() ? m_slowPerSecond->m_duration : 0.f;
    out_string += StringUtils::StringF("DPS(%.1f) PPS(%.1f) BPS(%.1f) SPS(%.1f)\n", damage, poison, burn, slow);
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

    m_cooldownComp = def.m_cooldownDef;
    m_targetingComp = def.m_targetingDef;
    m_critComp = def.m_critDef;
    m_onHitComp = def.m_onHitDef;
    m_chainComp = def.m_chainDef;
    m_multishotComp = def.m_multishotDef;
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

	int maxTargets = m_multishotComp.has_value() ? 1 + m_multishotComp->m_additionalTargets : 1;
	int maxChains = m_chainComp.has_value() ? m_chainComp->m_maxChains : 0;
	float chainDistance = m_chainComp.has_value() ? m_chainComp->m_chainDistance : 0.f;

    if (!m_targetingComp->FindTargets(context, maxTargets, maxChains, chainDistance))
    {
        // No targets in range, clamp cooldown
        m_cooldownComp->m_accumulatedTime = MathUtils::Clamp(m_cooldownComp->m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

    // Shoot at targets
    while (m_cooldownComp->m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp->m_accumulatedTime -= timeBetweenAttacks;

        for (int targetIndex = 0; targetIndex < m_targetingComp->m_targetChains.GetDimensions().x; ++targetIndex)
        {
            EntityID targetID = m_targetingComp->m_targetChains.Get(IntVec2(targetIndex, 0));

            SpawnInfo spawnInfo;
            spawnInfo.m_spawnPos = location;
            spawnInfo.m_spawnOrientation = 0.f;
            spawnInfo.m_def = EntityDef::GetEntityDef(m_projectileDefName);
            EntityID projectileID = SEntityFactory::SpawnEntity(context, spawnInfo);
            if (!context.IsValid(projectileID))
            {
                break;
            }

            // Copy ability data to proj, snapshotted with damage and effects already rolled.
            ASSERT_OR_DIE(context.HasComponent<CProjectile>(projectileID), "ProjectileHitAbility::Update - spawned projectile is missing CProjectile component.");
            CProjectile& projComp = projectileStorage[projectileID];

            projComp.m_targetID = targetID;
            projComp.m_targetPos = std::nullopt;
            projComp.m_accumulatedTime += m_cooldownComp->m_accumulatedTime;
            projComp.m_projSpeed = m_projSpeed;
            projComp.m_onHitComp = RollDamageAndEffects(rng);
        }
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
RolledOnHitComponent ProjectileHitAbility::RollDamageAndEffects(RandomNumberGenerator& rng) const
{
	RolledOnHitComponent hitResult;

	float critMultiplier = s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.has_value())
    {
        AbilityCritComponent const& critComp = m_critComp.value();
        float critRoll = rng.GetRandomFloatZeroToOne();
        critMultiplier += critComp.m_critMulti;
        didCrit = critRoll < critComp.m_critChance;
    }

    if (m_onHitComp.has_value())
    {
        AbilityOnHitComponent const& onHitComp = m_onHitComp.value();
        HitPayload& onHitPayload = hitResult.m_payload;
        onHitPayload.m_didCrit = didCrit;

        if (onHitComp.m_damageOnHit.has_value())
        {
            AbilityDamageComponent const& damageComp = onHitComp.m_damageOnHit.value();
            onHitPayload.m_damage = rng.GetRandomFloatInRange(damageComp.m_minDamage, damageComp.m_maxDamage);
            if (didCrit)
            {
                onHitPayload.m_damage *= critMultiplier;
            }
        }

        if (onHitComp.m_burnOnHit.has_value())
        {
            AbilityBurnComponent const& burnComp = onHitComp.m_burnOnHit.value();
            onHitPayload.m_burn = burnComp.m_burn;
            if (didCrit)
            {
                onHitPayload.m_burn *= critMultiplier;
            }
        }

        if (onHitComp.m_poisonOnHit.has_value())
        {
            AbilityPoisonComponent const& poisonComp = onHitComp.m_poisonOnHit.value();
            onHitPayload.m_poison = poisonComp.m_poison;
            if (didCrit)
            {
                onHitPayload.m_poison *= critMultiplier;
            }
        }

        if (onHitComp.m_slowOnHit.has_value())
        {
            AbilitySlowComponent const& slowComp = onHitComp.m_slowOnHit.value();
            onHitPayload.m_slowDuration = slowComp.m_duration;
		}

        if (onHitComp.m_aoeHitOnHit.has_value())
        {
            RolledAoEHitComponent aoeHitResult;
			HitPayload& aoeHitPayload = aoeHitResult.m_payload;
            aoeHitPayload.m_didCrit = didCrit;

            AbilityAoEHitComponent const& aoeHitComp = onHitComp.m_aoeHitOnHit.value();
            if (aoeHitComp.m_damageOnHit.has_value())
            {
                AbilityDamageComponent const& aoeDamageComp = aoeHitComp.m_damageOnHit.value();
                aoeHitPayload.m_damage = rng.GetRandomFloatInRange(aoeDamageComp.m_minDamage, aoeDamageComp.m_maxDamage);
                if (didCrit)
                {
                    aoeHitPayload.m_damage *= critMultiplier;
                }
            }

            if (aoeHitComp.m_burnOnHit.has_value())
            {
                AbilityBurnComponent const& aoeBurnComp = aoeHitComp.m_burnOnHit.value();
                aoeHitPayload.m_burn = aoeBurnComp.m_burn;
                if (didCrit)
                {
                    aoeHitPayload.m_burn *= critMultiplier;
                }
            }

            if (aoeHitComp.m_poisonOnHit.has_value())
            {
                AbilityPoisonComponent const& aoePoisonComp = aoeHitComp.m_poisonOnHit.value();
                aoeHitPayload.m_poison = aoePoisonComp.m_poison;
                if (didCrit)
                {
                    aoeHitPayload.m_poison *= critMultiplier;
                }
            }

            if (aoeHitComp.m_slowOnHit.has_value())
            {
                AbilitySlowComponent const& aoeSlowComp = aoeHitComp.m_slowOnHit.value();
                aoeHitPayload.m_slowDuration = aoeSlowComp.m_duration;
			}

			hitResult.m_aoeHitOnHit = aoeHitResult;
        }

        if (m_onHitComp->m_aoeEffectOnHit.has_value())
        {
			hitResult.m_aoeEffectOnHit = m_onHitComp->m_aoeEffectOnHit.value();
        }
    }

	return hitResult;
}



//----------------------------------------------------------------------------------------------------------------------
AoEHitAbility::AoEHitAbility(AoEHitAbilityDef const& def) : Ability(def)
{
	m_cooldownComp = def.m_cooldownDef;
	m_targetingComp = def.m_targetingDef;
	m_critComp = def.m_critDef;
	m_aoeHitComp = def.m_aoeHitDef;
	m_aoeEffectComp = def.m_aoeEffectDef;
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

        HitPayload const& payload = RollDamageAndEffects(rng);
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
HitPayload AoEHitAbility::RollDamageAndEffects(RandomNumberGenerator& rng) const
{
	HitPayload payload;

    float critMultiplier = s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.has_value())
    {
        AbilityCritComponent const& critComp = m_critComp.value();
        float critRoll = rng.GetRandomFloatZeroToOne();
        critMultiplier += critComp.m_critMulti;
        didCrit = critRoll < critComp.m_critChance;
		payload.m_didCrit = didCrit;
    }

    if (m_aoeHitComp.has_value())
    {
        AbilityAoEHitComponent const& aoeHitComp = m_aoeHitComp.value();
        if (aoeHitComp.m_damageOnHit.has_value())
        {
            AbilityDamageComponent const& damageComp = aoeHitComp.m_damageOnHit.value();
            payload.m_damage = rng.GetRandomFloatInRange(damageComp.m_minDamage, damageComp.m_maxDamage);
            if (didCrit)
            {
                payload.m_damage *= critMultiplier;
            }
        }

        if (aoeHitComp.m_burnOnHit.has_value())
        {
            AbilityBurnComponent const& burnComp = aoeHitComp.m_burnOnHit.value();
            payload.m_burn = burnComp.m_burn;
            if (didCrit)
            {
                payload.m_burn *= critMultiplier;
            }
        }

        if (aoeHitComp.m_poisonOnHit.has_value())
        {
            AbilityPoisonComponent const& poisonComp = aoeHitComp.m_poisonOnHit.value();
            payload.m_poison = poisonComp.m_poison;
            if (didCrit)
            {
                payload.m_poison *= critMultiplier;
            }
        }

        if (aoeHitComp.m_slowOnHit.has_value())
        {
            AbilitySlowComponent const& slowComp = aoeHitComp.m_slowOnHit.value();
            payload.m_slowDuration = slowComp.m_duration;
		}
    }

    return payload;
}



//----------------------------------------------------------------------------------------------------------------------
PassiveAoEAbility::PassiveAoEAbility(PassiveAoEAbilityDef const& def) : Ability(def)
{
	m_targetingComp = def.m_targetingDef;
	m_aoeEffectComp = def.m_aoeEffectDef;
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
    m_targetingComp = def.m_targetingDef;
	m_onHitComp = def.m_onHitDef;
	m_renderComp = def.m_renderDef;
	m_chainComp = def.m_chainDef;
	m_multishotComp = def.m_multishotDef;
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

    int maxTargets = m_multishotComp.has_value() ? 1 + m_multishotComp->m_additionalTargets : 1;
    int maxChains = m_chainComp.has_value() ? m_chainComp->m_maxChains : 0;
    float chainDistance = m_chainComp.has_value() ? m_chainComp->m_chainDistance : 0.f;

    if (!m_targetingComp->FindTargets(context, maxTargets, maxChains, chainDistance))
    {
        return;
    }

    HitPayload const payload = RollDamageAndEffects(context.m_deltaSeconds);

    for (int targetIndex = 0; targetIndex < m_targetingComp->m_targetChains.GetDimensions().x; ++targetIndex)
    {
        float chainPayloadMulti = 1.f;

        for (int chainIndex = 0; chainIndex < m_targetingComp->m_targetChains.GetDimensions().y; ++chainIndex)
        {
            EntityID target = m_targetingComp->m_targetChains.Get(IntVec2(targetIndex, chainIndex));
            if (target == EntityID::Invalid)
            {
                break;
            }

			HitPayload chainPayload = payload;
            chainPayload *= chainPayloadMulti;

            if (m_chainComp.has_value())
            {
                chainPayloadMulti *= m_chainComp->m_chainPayloadMulti;
            }

            if (chainPayload.IsRelevantToHealth() && context.HasComponentsUnsafe(target.GetIndex(), healthBit))
            {
                CHealth& healthComp = context.GetArrayStorage<CHealth>()[target];
                healthComp.TakePayload(chainPayload);
            }

            if (chainPayload.IsRelevantToTime() && context.HasComponentsUnsafe(target.GetIndex(), timeBit))
            {
                CTime& timeComp = context.GetArrayStorage<CTime>()[target];
                timeComp.m_remainingSlowDuration += chainPayload.m_slowDuration;
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
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Render(SystemContext const& context, Vec2 const& location) const
{
	ASSERT_OR_DIE(m_renderComp.has_value(), "LaserAbility::Render - m_renderComp is null.");

	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    VertexBuffer& vbo = *renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
    vbo.ClearVerts();

    for (int targetIndex = 0; targetIndex < m_targetingComp->m_targetChains.GetDimensions().x; ++targetIndex)
    {
        Vec2 currentChainStartLocation = location;

        for (int chainIndex = 0; chainIndex < m_targetingComp->m_targetChains.GetDimensions().y; ++chainIndex)
        {
            EntityID target = m_targetingComp->m_targetChains.Get(IntVec2(targetIndex, chainIndex));
            if (!context.IsValid(target))
            {
                continue;
            }

            if (CTransform const* transform = context.GetComponent<CTransform>(target))
            {
                VertexUtils::AddVertsForLine2D(vbo, currentChainStartLocation, transform->m_pos, 0.25f, m_renderComp->m_tint, m_renderComp->m_depth);
                currentChainStartLocation = transform->m_pos;
            }
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



//----------------------------------------------------------------------------------------------------------------------
HitPayload LaserAbility::RollDamageAndEffects(float deltaSeconds) const
{
    HitPayload result;
    result.m_didCrit = false;
	result.m_damage = m_onHitComp.has_value() && m_onHitComp->m_damageOnHit.has_value() ? m_onHitComp->m_damageOnHit->m_maxDamage : 0.f;
    result.m_burn = m_onHitComp.has_value() && m_onHitComp->m_burnOnHit.has_value() ? m_onHitComp->m_burnOnHit->m_burn : 0.f;
    result.m_poison = m_onHitComp.has_value() && m_onHitComp->m_poisonOnHit.has_value() ? m_onHitComp->m_poisonOnHit->m_poison : 0.f;
    result.m_slowDuration = m_onHitComp.has_value() && m_onHitComp->m_slowOnHit.has_value() ? m_onHitComp->m_slowOnHit->m_duration : 0.f;
    result *= deltaSeconds;
	return result;
}
