// Bradley Christensen - 2022-2026
#include "AbilityComponents.h"
#include "AbilityComponentDefs.h"
#include "Ability.h"
#include "CAbility.h"
#include "CHealth.h"
#include "CCollision.h"
#include "CPlaceable.h"
#include "CTransform.h"
#include "SCCollision.h"
#include "SCFlowField.h"
#include "SCWorld.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/MathUtils.h"
#include <algorithm>



//----------------------------------------------------------------------------------------------------------------------
AbilityTargetingComponent::AbilityTargetingComponent(AbilityTargetingComponentDef const& def)
{
	m_maxRange = def.m_maxRange;
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityTargetingComponent::GetMaxRange(CAbility const& ability) const
{
	float rangeModifier = 1.f + ability.m_attributes.GetValue(EAbilityAttribute::Range_Multi);
    return m_maxRange * rangeModifier;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityTargetingComponent::UpdateCachedTiles(SystemContext const& context, CAbility const& ability, Vec2 const& location)
{
    if (location != m_cachedLocation)
    {
		m_needsCacheUpdate = 1;
    }

    if (m_needsCacheUpdate)
    {
        SCWorld const& world = context.GetSingletonConst<SCWorld>();

		m_cachedLocation = location;
		m_needsCacheUpdate = 0;

        m_cachedTilesInRange.clear();

		float maxRange = GetMaxRange(ability);

        world.ForEachPathTileInRange(location, 0.f, maxRange, [&](IntVec2 const& worldCoords)
        {
            m_cachedTilesInRange.push_back(worldCoords);
            return true;
        });

        if (m_targetingMode == AbilityTargetingMode::ClosestToGoal)
        {
            // Sort by distance so that when we go to find targets, the best target is first in the list.
            SCFlowField const& flowfield = context.GetSingletonConst<SCFlowField>();
            std::sort(m_cachedTilesInRange.begin(), m_cachedTilesInRange.end(), [&](IntVec2 const& a, IntVec2 const& b)
            {
                int aTileIndex = world.m_tiles.GetIndexForCoords(a);
                int bTileIndex = world.m_tiles.GetIndexForCoords(b);
                return flowfield.m_toGoalFlowField.m_distanceField.Get(aTileIndex) < flowfield.m_toGoalFlowField.m_distanceField.Get(bTileIndex);
            });
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityTargetingComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float maxRange = GetMaxRange(*debugContext.m_abilityComp);
    debugContext.m_debugString += StringUtils::StringF("Range: %.1f\n", maxRange);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoETargetingComponent::AbilityAoETargetingComponent(AbilityTargetingComponentDef const& def) : AbilityTargetingComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
float AbilityAoETargetingComponent::GetMaxRange(CAbility const& ability) const
{
    float rangeModifier = 1.f;
    rangeModifier += ability.m_attributes.GetValue(EAbilityAttribute::Range_Multi);
    rangeModifier += ability.m_attributes.GetValue(EAbilityAttribute::AreaOfEffect_Multi);
    return m_maxRange * rangeModifier;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoETargetingComponent::FindTargets(SystemContext const& context, CAbility const& ability, int maxTargets /*= -1*/)
{
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	SCCollision const& scCollision = context.GetSingletonConst<SCCollision>();
	CollisionLayer const& enemyLayer = scCollision.GetCollisionLayer(CollisionChannel::Enemy);
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& transformStorage = context.GetArrayStorageConst<CTransform>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();

	float maxRange = GetMaxRange(ability);

    m_targets.clear();

    uint32_t splashCounter = Ability::GetNextSplashId();

    for (IntVec2 const& cachedPathTile : m_cachedTilesInRange)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);

        CollisionBucket const& tileBucket = enemyLayer[tileIndex];

        for (EntityID entityID : tileBucket)
        {
            CHealth& healthComp = healthStorage[entityID];
			if (healthComp.m_lastProcessedBy == splashCounter)
			{
                // This targeting component has already processed this entity, skip
				continue;
			}

			healthComp.m_lastProcessedBy = splashCounter;

            if (!healthComp.GetIsTargetable() || healthComp.GetHealthReachedZero())
            {
                continue;
            }

            CTransform const& transform = transformStorage[entityID];
            CCollision const& collision = collisionStorage[entityID];
            float range = collision.m_radius + maxRange; // Add target's radius to the range
            float rangeSquared = range * range;
            float distSquared = MathUtils::GetDistanceSquared2D(transform.m_pos, m_cachedLocation);
            if (distSquared > rangeSquared)
            {
                continue;
            }

            m_targets.push_back(entityID);

            if (maxTargets > 0 && m_targets.size() >= maxTargets)
            {
                return true;
            }
        }
    }

	return !m_targets.empty();
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAdjacentTargetingComponent::FindTargets(SystemContext const& context, EntityID owner)
{
    SCWorld const& world = context.GetSingletonConst<SCWorld>();
    SCCollision const& scCollision = context.GetSingletonConst<SCCollision>();
    CollisionLayer const& buildingLayer = scCollision.GetCollisionLayer(CollisionChannel::Building);

    m_targets.clear();

	CPlaceable const& placeableComp = *context.GetComponentConst<CPlaceable>(owner);

    world.ForEachPlayableTileInRegion(placeableComp.m_botLeftTile, placeableComp.m_botLeftTile + placeableComp.m_dims - IntVec2::OneVector, [&](IntVec2 const& worldCoords)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
        CollisionBucket const& tileBucket = buildingLayer[tileIndex];
        for (EntityID entityID : tileBucket)
        {
            if (entityID == owner)
            {
                continue;
            }
            m_targets.insert(entityID);
        }
        return true;
    });

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
        for (IntVec2 const& cachedPathTile : m_cachedTilesInRange)
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

	auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();

    CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);

    float maxDistanceSquared = maxDistance * maxDistance;

    EntityID result = EntityID::Invalid;

	world.ForEachPathTileOverlappingCircle(pos, maxDistance, [&](IntVec2 const& worldCoords)
	{
		int tileIndex = world.m_tiles.GetIndexForCoords(worldCoords);
		CollisionBucket const& tileBucket = enemyLayer[tileIndex];
		for (EntityID entityID : tileBucket)
		{
			if (m_targetChains.Contains(entityID))
			{
				continue;
			}
			CTransform const& transformComp = transStorage[entityID];
			float distSquared = MathUtils::GetDistanceSquared2D(pos, transformComp.m_pos);
			if (distSquared > maxDistanceSquared)
			{
				continue;
			}
			CHealth const& healthComp = healthStorage[entityID];
			if (healthComp.GetIsTargetable() && !healthComp.GetHealthReachedZero())
			{
				result = entityID;
                return false; // stop iterating
			}
		}
		return true; // keep iterating
	});

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCooldownComponent::AbilityCooldownComponent(AbilityCooldownComponentDef const& def)
{
	m_cooldownSeconds = def.m_cooldownSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityCooldownComponent::GetCooldown(CAbility const& ability) const
{
	float attackSpeedModifier = ability.m_attributes.GetValue(EAbilityAttribute::AttackSpeed_Multi);
	float attackSpeed = 1.f + attackSpeedModifier;
    return m_cooldownSeconds * (1.f / attackSpeed);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	ASSERT_OR_DIE(debugContext.m_abilityComp, "AbilityCooldownComponent::AppendDebugString - debugContext.m_abilityComp is null.");
	float cooldown = GetCooldown(*debugContext.m_abilityComp);
	cooldown /= debugContext.m_entityTimeDilation;
    debugContext.m_debugString += StringUtils::StringF("Cooldown: %.3f\n", cooldown);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCritComponent::AbilityCritComponent(AbilityCritComponentDef const& def)
{
    m_critChance = def.m_critChance;
	m_critMulti = def.m_critMulti;
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityCritComponent::GetCritChance(CAbility const& ability) const
{
	return m_critChance + ability.m_attributes.GetValue(EAbilityAttribute::CritChance_Add);
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityCritComponent::GetCritMultiplier(CAbility const& ability) const
{
    float critMultiplier = StaticGameSettings::s_baseCritMultiplier;
	critMultiplier += m_critMulti;
	critMultiplier += ability.m_attributes.GetValue(EAbilityAttribute::CritMulti_Multi);
	return critMultiplier;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCritComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (m_critChance <= 0.f)
    {
        return;
	}
    debugContext.m_debugString += StringUtils::StringF("Crit Chance: %.1f%%\n", m_critChance * 100.f);
    debugContext.m_debugString += StringUtils::StringF("Crit Mult: %.1f\n", StaticGameSettings::s_baseCritMultiplier + m_critMulti);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityScalingComponent::AbilityScalingComponent(AbilityScalingComponentDef const& def)
{
    m_physical = def.m_physical;
	m_burn = def.m_burn;
	m_poison = def.m_poison;
	m_slow = def.m_slow;
	m_haste = def.m_haste;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload AbilityScalingComponent::CalculateHitPayload(CAbility const& ability, bool didCrit, float critMultiplier) const
{
    HitPayload result;
	result.m_didCrit = didCrit;
    result.m_physical = ability.m_attributes.GetValue(EAbilityAttribute::Physical) * m_physical;
    result.m_burn = ability.m_attributes.GetValue(EAbilityAttribute::Burn) * m_burn;
    result.m_poison = ability.m_attributes.GetValue(EAbilityAttribute::Poison) * m_poison;
    result.m_slowDuration = ability.m_attributes.GetValue(EAbilityAttribute::SlowDuration) * m_slow;
    result.m_hasteDuration = ability.m_attributes.GetValue(EAbilityAttribute::HasteDuration) * m_haste;

    if (didCrit)
    {
        result.m_physical *= critMultiplier;
        result.m_burn *= critMultiplier;
        result.m_poison *= critMultiplier;
    }

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload AbilityScalingComponent::CalculateDotPayload(CAbility const& ability, float deltaSeconds) const
{
    // DoT cannot crit
    HitPayload result = CalculateHitPayload(ability);
	result *= deltaSeconds;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityScalingComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	if (!IsRelevant())
	{
		return;
	}

    if (m_physical > 0.f)
    {
		debugContext.m_debugString += StringUtils::StringF("Phys (%.2f) ", m_physical);
    }
	if (m_burn > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Burn (%.2f) ", m_burn);
	}
	if (m_poison > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Poison (%.2f) ", m_poison);
	}
	if (m_slow > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Slow (%.2f) ", m_slow);
	}
	if (m_haste > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("Haste (%.2f) ", m_haste);
	}
	debugContext.m_debugString += '\n';
}



//----------------------------------------------------------------------------------------------------------------------
AbilityChainComponent::AbilityChainComponent(AbilityChainComponentDef const& def)
{
    m_chainChance = def.m_chainChance;
    m_chainDistance = def.m_chainDistance;
	m_chainPayloadMulti = def.m_chainPayloadMulti;
    m_maxChains = def.m_maxChains;
}



//----------------------------------------------------------------------------------------------------------------------
int AbilityChainComponent::GetMaxChains(CAbility const& ability) const
{
	return m_maxChains + static_cast<int>(ability.m_attributes.GetValue(EAbilityAttribute::ChainCount_Add));
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityChainComponent::GetChainDistance(CAbility const& ability) const
{
	return m_chainDistance + ability.m_attributes.GetValue(EAbilityAttribute::ChainDistance_Multi);
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityChainComponent::GetChainPayloadMulti(CAbility const&) const
{
    return m_chainPayloadMulti; // + ability.m_attributes.GetValue(EAbilityAttribute::ChainPayloadMulti); // todo:
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityChainComponent::GetChainChance(CAbility const& ability) const
{
	return m_chainChance + ability.m_attributes.GetValue(EAbilityAttribute::ChainChance_Add);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityChainComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	if (m_maxChains <= 0)
	{
		return;
	}
    debugContext.m_debugString += StringUtils::StringF("Chain Chance: %.2f\n", m_chainChance * 100.f);
    debugContext.m_debugString += StringUtils::StringF("Max Chains: %d\n", m_maxChains);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityMultishotComponent::AbilityMultishotComponent(AbilityMultishotComponentDef const& def)
{
	m_additionalTargets = def.m_additionalTargets;
}



//----------------------------------------------------------------------------------------------------------------------
int AbilityMultishotComponent::GetAdditionalTargets(CAbility const& ability) const
{
    return m_additionalTargets + static_cast<int>(ability.m_attributes.GetValue(EAbilityAttribute::MultishotCount_Add));
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityMultishotComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (m_additionalTargets <= 0)
    {
        return;
    }

	debugContext.m_debugString += StringUtils::StringF("Additional Targets: %d\n", m_additionalTargets);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityRenderComponent::AbilityRenderComponent(AbilityRenderComponentDef const& def)
{
    m_tint = def.m_tint;
	m_depth = def.m_depth;
	m_renderDuration = def.m_renderDuration;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityRenderComponent::AppendDebugString(EntityDebugContext&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
bool RolledAoEHitComponent::IsRelevant() const
{
    return m_radius > 0.f && m_payload.HasValue();
}



//----------------------------------------------------------------------------------------------------------------------
void RolledAoEHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
        return;
    }
    debugContext.m_debugString += StringUtils::StringF("---Rolled AOE Hit---\n");
    m_payload.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEHitComponent::AbilityAoEHitComponent(AbilityAoEHitComponentDef const& def)
{
    m_radius = def.m_radius;
    m_scaling = def.m_scaling;
    m_renderComp = def.m_renderDef;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoEHitComponent::IsRelevant() const
{
	float radius = m_radius;
	bool hasRadius = radius > 0.f;
	if (!hasRadius)
	{
		return false;
	}

	bool hasPayload = m_scaling.IsRelevant();
    return hasPayload;
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityAoEHitComponent::GetRadius(CAbility const& ability) const
{
	float areaModifier = 1.f + ability.m_attributes.GetValue(EAbilityAttribute::AreaOfEffect_Multi);
	return m_radius * areaModifier;
}



//----------------------------------------------------------------------------------------------------------------------
RolledAoEHitComponent AbilityAoEHitComponent::CalculateSplashPayload(CAbility const& ability, bool didCrit, float critMultiplier) const
{
    RolledAoEHitComponent result;
	result.m_radius = GetRadius(ability);
	result.m_payload = m_scaling.CalculateHitPayload(ability, didCrit, critMultiplier);
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
RolledAoEHitComponent AbilityAoEHitComponent::CalculateDotPayload(CAbility const& ability, float deltaSeconds) const
{
    RolledAoEHitComponent result;
    result.m_radius = GetRadius(ability);
    result.m_payload = m_scaling.CalculateDotPayload(ability, deltaSeconds);
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
		return;
	}   

    debugContext.m_debugString += StringUtils::StringF("---AOE Hit---\n");

	float radius = m_radius;
    if (radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", radius);
    }
	m_scaling.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
bool RolledAoEEffectComponent::IsRelevant() const
{
	return m_payload.HasValue() && m_radius > 0.f;
}



//----------------------------------------------------------------------------------------------------------------------
void RolledAoEEffectComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	m_payload.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEEffectComponent::AbilityAoEEffectComponent(AbilityAoEEffectComponentDef const& def)
{
    m_aoeEffectDefName = def.m_aoeEffectDefName;
    m_radius = def.m_radius;
    m_durationSeconds = def.m_durationSeconds;
	m_scaling = def.m_scaling;
	m_renderComp = def.m_renderDef;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoEEffectComponent::IsRelevant() const
{
	if (m_aoeEffectDefName == Name::Invalid)
	{
		return false;
	}

	bool hasPayload = m_scaling.IsRelevant();
	return hasPayload;
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityAoEEffectComponent::GetRadius(CAbility const& ability) const
{
    float areaModifier = 1.f + ability.m_attributes.GetValue(EAbilityAttribute::AreaOfEffect_Multi);
    return m_radius * areaModifier;
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityAoEEffectComponent::GetDuration(CAbility const&) const
{
    return m_durationSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
RolledAoEEffectComponent AbilityAoEEffectComponent::CalculatePuddlePayload(CAbility const& ability) const
{
    RolledAoEEffectComponent result;
    result.m_aoeEffectDefName = m_aoeEffectDefName;
    result.m_radius = GetRadius(ability);
    result.m_durationSeconds = GetDuration(ability);
    result.m_payload = m_scaling.CalculateHitPayload(ability);
    result.m_renderComp = m_renderComp;
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
RolledAoEEffectComponent AbilityAoEEffectComponent::CalculateDotPayload(CAbility const& ability, float deltaSeconds) const
{
    RolledAoEEffectComponent result;
	result.m_aoeEffectDefName = m_aoeEffectDefName;
	result.m_radius = GetRadius(ability);
	result.m_durationSeconds = GetDuration(ability);
	result.m_payload = m_scaling.CalculateDotPayload(ability, deltaSeconds);
    result.m_renderComp = m_renderComp;
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEEffectComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
        return;
    }

	float radius = GetRadius(*debugContext.m_abilityComp);
	float duration = GetDuration(*debugContext.m_abilityComp);

    debugContext.m_debugString += StringUtils::StringF("---AOE Effect---\n", radius);
    if (radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", radius);
	}
    debugContext.m_debugString += StringUtils::StringF("Duration: %.1f\n", duration);	

	m_scaling.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
bool RolledOnHitComponent::IsRelevant() const
{
    return m_payload.HasValue() || m_aoeHitOnHit.IsRelevant() || m_aoeEffectOnHit.IsRelevant();
}



//----------------------------------------------------------------------------------------------------------------------
void RolledOnHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
        return;
    }
    debugContext.m_debugString += StringUtils::StringF("---Rolled On Hit---\n");
    m_payload.AppendDebugString(debugContext);
    m_aoeHitOnHit.AppendDebugString(debugContext);
    m_aoeEffectOnHit.AppendDebugString(debugContext);
}




//----------------------------------------------------------------------------------------------------------------------
AbilityOnHitComponent::AbilityOnHitComponent(AbilityOnHitComponentDef const& def)
{
    m_scaling = def.m_scaling;
	m_aoeHitOnHit = def.m_aoeHitOnHit;
	m_aoeEffectOnHit = def.m_aoeEffectOnHit;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityOnHitComponent::IsRelevant() const
{
	bool hasPayload = m_scaling.IsRelevant() || m_aoeHitOnHit.IsRelevant() || m_aoeEffectOnHit.IsRelevant();
    return hasPayload;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityOnHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
        return;
    }

    debugContext.m_debugString += StringUtils::StringF("---Hit---\n");
	m_scaling.AppendDebugString(debugContext);
    m_aoeHitOnHit.AppendDebugString(debugContext);
    m_aoeEffectOnHit.AppendDebugString(debugContext);
}