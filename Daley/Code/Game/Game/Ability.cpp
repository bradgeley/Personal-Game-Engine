// Bradley Christensen - 2022-2026
#include "Ability.h"
#include "AbilityDef.h"
#include "CProjectile.h"
#include "CTime.h"
#include "EntityDef.h"
#include "SCCollision.h"
#include "SCFlowField.h"
#include "SCWorld.h"
#include "SEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/Grid.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AbilityTargetingComponent::AbilityTargetingComponent(AbilityTargetingComponentDef const& def)
{
    m_minRange = def.m_minRange;
	m_maxRange = def.m_maxRange;
	m_abilityTargetFlags = def.m_abilityTargetFlags;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityTargetingComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();
	if (def.m_abilityAttribute == TowerAbilityAttribute::Range)
	{
		m_rangeMultiplier += modifier.GetValue();
		m_needsCacheUpdate = 1;
		return true;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityTargetingComponent::UpdateCachedTiles(SystemContext const& context, Vec2 const& location)
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

		float minRange = GetMinRange();
		float maxRange = GetMaxRange();

        // Note: Tower case covers Tower+Enemy case, but Enemy case does not. So we check tower first.
        if (m_abilityTargetFlags & (uint8_t) AbilityTargetFlags::Tower)
        {
            world.ForEachPlayableTileOverlappingCircle(location, maxRange, [&](IntVec2 const& worldCoords)
            {
                m_cachedTilesInRange.push_back(worldCoords);
                return true;
            });
        }
        else if (m_abilityTargetFlags & (uint8_t) AbilityTargetFlags::Enemy)
        {
            world.ForEachPathTileInRange(location, minRange, maxRange, [&](IntVec2 const& worldCoords)
            {
                m_cachedTilesInRange.push_back(worldCoords);
                return true;
            });
        }

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
    float minRange = GetMinRange();
	float maxRange = GetMaxRange();
    if (minRange > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Range: %.1f - %.1f (x%.1f)\n", minRange, maxRange, m_rangeMultiplier);
    }
    else
    {
        debugContext.m_debugString += StringUtils::StringF("Range: %.1f (x%.1f)\n", maxRange, m_rangeMultiplier);
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
	SCCollision const& scCollision = context.GetSingletonConst<SCCollision>();
	CollisionLayer const& enemyLayer = scCollision.GetCollisionLayer(CollisionChannel::Enemy);
	CollisionLayer const& buildingLayer = scCollision.GetCollisionLayer(CollisionChannel::Building);
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	auto& transformStorage = context.GetArrayStorageConst<CTransform>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();
	BitMask healthBit = context.GetComponentBitMask<CHealth>();

	float maxRange = GetMaxRange();

    m_targets.clear();

    for (IntVec2 const& cachedPathTile : m_cachedTilesInRange)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);

        if (m_abilityTargetFlags & (uint8_t) AbilityTargetFlags::Enemy)
        {
            CollisionBucket const& tileBucket = enemyLayer[tileIndex];

            for (EntityID entityID : tileBucket)
            {
                if (m_targets.find(entityID) != m_targets.end())
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

		if (m_abilityTargetFlags & (uint8_t) AbilityTargetFlags::Tower)
		{
			CollisionBucket const& tileBucket = buildingLayer[tileIndex];

			for (EntityID entityID : tileBucket)
			{
				if (m_targets.find(entityID) != m_targets.end())
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

				m_targets.insert(entityID);
				if (maxTargets > 0 && m_targets.size() >= maxTargets)
				{
					return true;
				}
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
float AbilityCooldownComponent::GetCooldown() const
{
	float cooldown = m_cooldownSeconds / (1.f + m_attackSpeedIncrease);
    return cooldown;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityCooldownComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();
    if (def.m_abilityAttribute == TowerAbilityAttribute::AttackSpeed)
    {
		m_attackSpeedIncrease += modifier.GetValue();
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float cooldown = GetCooldown();
	cooldown /= debugContext.m_entityTimeDilation;
    debugContext.m_debugString += StringUtils::StringF("Cooldown: %.3f (x%.2f)\n", cooldown, 1.f + m_attackSpeedIncrease);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCritComponent::AbilityCritComponent(AbilityCritComponentDef const& def)
{
    m_critChance = def.m_critChance;
	m_critMulti = def.m_critMulti;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityCritComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();
    if (def.m_abilityAttribute == TowerAbilityAttribute::CritChance)
    {
		m_critChance += modifier.GetValue();
		return true;
    }
    else if (def.m_abilityAttribute == TowerAbilityAttribute::CritDamage)
    {
		m_critMulti += modifier.GetValue();
        return true;
    }
    return false;
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
AbilityDamageComponent::AbilityDamageComponent(AbilityDamageComponentDef const& def)
{
    m_minDamage = def.m_minDamage;
	m_maxDamage = def.m_maxDamage;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityDamageComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::Damage)
	{
		m_damageMultiplier += modifier.GetValue();
		return true;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityDamageComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float minDamage = GetMinDamage();
	float maxDamage = GetMaxDamage();
    if (minDamage <= 0.f && maxDamage <= 0.f)
    {
        return;
    }

    if (MathUtils::IsNearlyEqual(minDamage, maxDamage))
    {
        debugContext.m_debugString += StringUtils::StringF("Damage: %.1f\n", minDamage);
    }
    else
    {
        debugContext.m_debugString += StringUtils::StringF("Damage: %.1f - %.1f\n", minDamage, maxDamage);
    }
}



//----------------------------------------------------------------------------------------------------------------------
AbilityBurnComponent::AbilityBurnComponent(AbilityBurnComponentDef const& def)
{
	m_burn = def.m_burn;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityBurnComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    if (def.m_abilityAttribute == TowerAbilityAttribute::Burn)
    {
        m_burnMultiplier += modifier.GetValue();
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityBurnComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float burn = GetBurn();
    if (burn <= 0.f)
    {
        return;
	}
    debugContext.m_debugString += StringUtils::StringF("Burn: %.1f\n", burn);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityPoisonComponent::AbilityPoisonComponent(AbilityPoisonComponentDef const& def)
{
	m_poison = def.m_poison;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityPoisonComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    if (def.m_abilityAttribute == TowerAbilityAttribute::Poison)
    {
        m_poisonMultiplier += modifier.GetValue();
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityPoisonComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float poison = GetPoison();
    if (poison <= 0.f)
    {
        return;
    }
    debugContext.m_debugString += StringUtils::StringF("Poison: %.1f\n", poison);
}



//----------------------------------------------------------------------------------------------------------------------
AbilitySlowComponent::AbilitySlowComponent(AbilitySlowComponentDef const& def)
{
	m_duration = def.m_duration;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilitySlowComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    if (def.m_abilityAttribute == TowerAbilityAttribute::Slow)
    {
        m_durationMultiplier += modifier.GetValue();
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilitySlowComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float duration = GetDuration();
    if (duration <= 0.f)
    {
        return;
	}
    debugContext.m_debugString += StringUtils::StringF("Slow: %.1f\n", duration);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityHasteComponent::AbilityHasteComponent(AbilityHasteComponentDef const& def)
{
	m_duration = def.m_duration;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityHasteComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::Haste)
	{
		m_durationMultiplier += modifier.GetValue();
		return true;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityHasteComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float duration = GetDuration();
    if (duration <= 0.f)
    {
        return;
    }
    debugContext.m_debugString += StringUtils::StringF("Haste: %.1f\n", duration);
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
bool AbilityChainComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::NumChains)
	{
		int chainIncrease = MathUtils::RoundF(modifier.GetValue());
		m_maxChains += chainIncrease;
		if (m_maxChains > StaticGameSettings::s_maxChainTargets)
		{
			DevConsoleUtils::LogWarning("AbilityChainComponent::ApplyModifier: m_maxChains (%d) exceeded StaticGameSettings::s_maxChainTargets (%d). Clamping to max.", m_maxChains, StaticGameSettings::s_maxChainTargets);
			m_maxChains = StaticGameSettings::s_maxChainTargets;
		}
		return true;
	}
	return false;
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
bool AbilityMultishotComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::Multishot)
	{
		int additionalTargetsIncrease = MathUtils::RoundF(modifier.GetValue());
		m_additionalTargets += additionalTargetsIncrease;
		return true;
	}
	return false;
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
AbilityOnHitComponent::AbilityOnHitComponent(AbilityOnHitComponentDef const& def)
{
    m_damageOnHit       = def.m_damageOnHit.has_value() ? *def.m_damageOnHit : AbilityDamageComponent();
	m_poisonOnHit       = def.m_poisonOnHit.has_value() ? *def.m_poisonOnHit : AbilityPoisonComponent();
	m_burnOnHit         = def.m_burnOnHit.has_value() ? *def.m_burnOnHit : AbilityBurnComponent();
	m_aoeHitOnHit       = def.m_aoeHitOnHit.has_value() ? *def.m_aoeHitOnHit : AbilityAoEHitComponent();
	m_aoeEffectOnHit    = def.m_aoeEffectOnHit.has_value() ? *def.m_aoeEffectOnHit : AbilityAoEEffectComponent();
	m_slowOnHit         = def.m_slowOnHit.has_value() ? *def.m_slowOnHit : AbilitySlowComponent();
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityOnHitComponent::IsRelevant() const
{
	bool hasPayload = m_damageOnHit.IsRelevant() || m_poisonOnHit.IsRelevant() || m_burnOnHit.IsRelevant() || m_aoeHitOnHit.IsRelevant() || m_aoeEffectOnHit.IsRelevant() || m_slowOnHit.IsRelevant();
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

	float minDamage = m_damageOnHit.GetMinDamage();
	float maxDamage = m_damageOnHit.GetMaxDamage();
	if (minDamage > 0.f || maxDamage > 0.f)
	{
        debugContext.m_debugString += StringUtils::StringF("D(%.1f-%.1f)", minDamage, maxDamage);
	}

	float burn = m_burnOnHit.GetBurn();
	if (burn > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF(" B(%.1f)", burn);
	}

	float poison = m_poisonOnHit.GetPoison();
	if (poison > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF(" P(%.1f)", poison);
	}

	float slow = m_slowOnHit.GetDuration();
	if (slow > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF(" S(%.1f)", slow);
	}
	debugContext.m_debugString += '\n';

    m_aoeHitOnHit.AppendDebugString(debugContext);
    m_aoeEffectOnHit.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityOnHitComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	bool applied = false;
	applied |= m_damageOnHit.ApplyModifier(modifier);
	applied |= m_burnOnHit.ApplyModifier(modifier);
	applied |= m_poisonOnHit.ApplyModifier(modifier);
	applied |= m_slowOnHit.ApplyModifier(modifier);
	applied |= m_aoeHitOnHit.ApplyModifier(modifier);
	applied |= m_aoeEffectOnHit.ApplyModifier(modifier);
	return applied;
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
AbilityAoEHitComponent::AbilityAoEHitComponent(AbilityAoEHitComponentDef const& def)
{
    m_radius = def.m_radius;
    m_damageOnHit = def.m_damageOnHit.has_value() ? *def.m_damageOnHit : AbilityDamageComponent();
    m_poisonOnHit = def.m_poisonOnHit.has_value() ? *def.m_poisonOnHit : AbilityPoisonComponent();
	m_burnOnHit = def.m_burnOnHit.has_value() ? *def.m_burnOnHit : AbilityBurnComponent();
	m_slowOnHit = def.m_slowOnHit.has_value() ? *def.m_slowOnHit : AbilitySlowComponent();
	m_hasteOnHit = def.m_hasteOnHit.has_value() ? *def.m_hasteOnHit : AbilityHasteComponent();
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoEHitComponent::IsRelevant() const
{
	float radius = GetRadius();
	bool hasRadius = radius > 0.f;
	if (!hasRadius)
	{
		return false;
	}

	bool hasPayload = m_damageOnHit.IsRelevant() || m_poisonOnHit.IsRelevant() || m_burnOnHit.IsRelevant() || m_slowOnHit.IsRelevant() || m_hasteOnHit.IsRelevant();
    return hasPayload;
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoEHitComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();

    bool applied = false;
    if (def.m_abilityAttribute == TowerAbilityAttribute::AoE)
    {
		m_radiusMultiplier += modifier.GetValue();
        applied = true;
    }

    applied |= m_damageOnHit.ApplyModifier(modifier);
    applied |= m_burnOnHit.ApplyModifier(modifier);
    applied |= m_poisonOnHit.ApplyModifier(modifier);
    applied |= m_slowOnHit.ApplyModifier(modifier);
	applied |= m_hasteOnHit.ApplyModifier(modifier);
    return applied;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
		return;
	}   

    debugContext.m_debugString += StringUtils::StringF("---AOE Hit---\n");

	float radius = GetRadius();
    if (radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", radius);
    }

    float minDamage = m_damageOnHit.GetMinDamage();
    float maxDamage = m_damageOnHit.GetMaxDamage();
	if (minDamage > 0.f || maxDamage > 0.f)
	{
        debugContext.m_debugString += StringUtils::StringF("D(%.1f-%.1f) ", minDamage, maxDamage);
	}

	float poison = m_poisonOnHit.GetPoison();
	if (poison > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("P(%.1f) ", poison);
	}

	float burn = m_burnOnHit.GetBurn();
	if (burn > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("B(%.1f) ", burn);
	}

	float slow = m_slowOnHit.GetDuration();
	if (slow > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("S(%.1f) ", slow);
	}

	float haste = m_hasteOnHit.GetDuration();
	if (haste > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF("H(%.1f) ", haste);
	}

	debugContext.m_debugString += '\n';
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEEffectComponent::AbilityAoEEffectComponent(AbilityAoEEffectComponentDef const& def)
{
    m_aoeEffectDefName = def.m_aoeEffectDefName;
    m_radius = def.m_radius;
    m_durationSeconds = def.m_durationSeconds;
    m_damagePerSecond = def.m_damagePerSecond.has_value() ? *def.m_damagePerSecond : AbilityDamageComponent();
    m_poisonPerSecond = def.m_poisonPerSecond.has_value() ? *def.m_poisonPerSecond : AbilityPoisonComponent();
	m_burnPerSecond = def.m_burnPerSecond.has_value() ? *def.m_burnPerSecond : AbilityBurnComponent();
	m_slowPerSecond = def.m_slowPerSecond.has_value() ? *def.m_slowPerSecond : AbilitySlowComponent();
	m_hastePerSecond = def.m_hastePerSecond.has_value() ? *def.m_hastePerSecond : AbilityHasteComponent();
	m_renderComp = def.m_renderDef.has_value() ? *def.m_renderDef : AbilityRenderComponent();
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoEEffectComponent::IsRelevant() const
{
	if (m_aoeEffectDefName == Name::Invalid)
	{
		return false;
	}

	if (GetRadius() <= 0.f)
	{
		return false;
	}

	bool hasPayload = m_damagePerSecond.IsRelevant() || m_poisonPerSecond.IsRelevant() || m_burnPerSecond.IsRelevant() || m_slowPerSecond.IsRelevant() || m_hastePerSecond.IsRelevant();
	return hasPayload;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEEffectComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
        return;
    }

	float radius = GetRadius();
	float duration = GetDuration();

    debugContext.m_debugString += StringUtils::StringF("---AOE Effect---\n", radius);
    if (radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", radius);
	}
    debugContext.m_debugString += StringUtils::StringF("Duration: %.1f\n", duration);	

    float dps = m_damagePerSecond.GetMaxDamage() * debugContext.m_entityTimeDilation;
    if (dps > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("DPS(%.1f)", dps);
    }

	float bps = m_burnPerSecond.GetBurn() * debugContext.m_entityTimeDilation;
	if (bps > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF(" BPS(%.1f)", bps);
	}

	float pps = m_poisonPerSecond.GetPoison() * debugContext.m_entityTimeDilation;
	if (pps > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF(" PPS(%.1f)", pps);
	}

	float sps = m_slowPerSecond.GetDuration() * debugContext.m_entityTimeDilation;
	if (sps > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF(" SPS(%.1f)", sps);
	}

	float hps = m_hastePerSecond.GetDuration() * debugContext.m_entityTimeDilation;
	if (hps > 0.f)
	{
		debugContext.m_debugString += StringUtils::StringF(" HPS(%.1f)", hps);
	}

	debugContext.m_debugString += '\n';
}



//----------------------------------------------------------------------------------------------------------------------
bool AbilityAoEEffectComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	bool applied = false;
	if (modifier.GetDef().m_abilityAttribute == TowerAbilityAttribute::AoE)
	{
		m_radiusMultiplier += modifier.GetValue();
		applied = true;
	}

	applied |= m_damagePerSecond.ApplyModifier(modifier);
	applied |= m_burnPerSecond.ApplyModifier(modifier);
	applied |= m_poisonPerSecond.ApplyModifier(modifier);
	applied |= m_slowPerSecond.ApplyModifier(modifier);
	applied |= m_hastePerSecond.ApplyModifier(modifier);
	return applied;
}



//----------------------------------------------------------------------------------------------------------------------
Ability::Ability(AbilityDef const& def) : m_abilityDef(&def)
{
}



//----------------------------------------------------------------------------------------------------------------------
void Ability::Initialize(SystemContext const& context, EntityID ownerEntityID)
{
    ASSERT_OR_DIE(context.IsValid(ownerEntityID), "Ability::Initialize - owner entity is invalid.");
    m_owner = ownerEntityID;
}



//----------------------------------------------------------------------------------------------------------------------
void Ability::Shutdown(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void Ability::AppendDebugString(EntityDebugContext& debugContext) const
{
    debugContext.m_debugString += StringUtils::StringF("Ability: %s\n", m_abilityDef ? m_abilityDef->m_name.ToCStr() : "Invalid");
}



//----------------------------------------------------------------------------------------------------------------------
bool Ability::ApplyModifier(TowerAbilityRunModifier const&)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitAbility::ProjectileHitAbility(ProjectileHitAbilityDef const& def) : Ability(def)
{
    m_projectileDefName = def.m_projectileDefName;
    m_projSpeed = def.m_projSpeed;

    m_cooldownComp = def.m_cooldownDef.has_value() ? *def.m_cooldownDef : AbilityCooldownComponent();
    m_targetingComp = def.m_targetingDef.has_value() ? *def.m_targetingDef : AbilityPrecisionTargetingComponent();
    m_critComp = def.m_critDef.has_value() ? *def.m_critDef : AbilityCritComponent();
    m_onHitComp = def.m_onHitDef.has_value() ? *def.m_onHitDef : AbilityOnHitComponent();
    m_chainComp = def.m_chainDef.has_value() ? *def.m_chainDef : AbilityChainComponent();
    m_multishotComp = def.m_multishotDef.has_value() ? *def.m_multishotDef : AbilityMultishotComponent();
};



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");

    float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    constexpr float maxAttacksPerSecond = 1000.f;
    constexpr float minTimeBetweenAttacks = 1.f / maxAttacksPerSecond;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown();
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, minTimeBetweenAttacks);

    if (m_cooldownComp.m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

	// Write Dependencies
	auto& projectileStorage = context.GetMapStorage<CProjectile>();
    RandomNumberGenerator& rng = *context.GetSingleton<SCRandomNumberGenerator>().GetRNG();

    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
    m_targetingComp.UpdateCachedTiles(context, location);

	int maxTargets = m_multishotComp.m_additionalTargets + 1;
	int maxChains = m_chainComp.m_maxChains;
	float chainDistance = m_chainComp.m_chainDistance;

    if (!m_targetingComp.FindTargets(context, maxTargets, maxChains, chainDistance))
    {
        // No targets in range, clamp cooldown
        m_cooldownComp.m_accumulatedTime = MathUtils::Clamp(m_cooldownComp.m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

	EntityDef const* projDef = EntityDef::GetEntityDef(m_projectileDefName);

    SpawnInfo spawnInfo;
    spawnInfo.m_spawnPos = location;
    spawnInfo.m_def = projDef;

    // Shoot at targets
    while (m_cooldownComp.m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp.m_accumulatedTime -= timeBetweenAttacks;

        for (int targetIndex = 0; targetIndex < m_targetingComp.m_targetChains.GetDimensions().x; ++targetIndex)
        {
            EntityID targetID = m_targetingComp.m_targetChains.Get(IntVec2(targetIndex, 0));

            EntityID projectileID = SEntityFactory::SpawnEntity(context, spawnInfo);
            if (!context.IsValid(projectileID))
            {
                break;
            }

            // Copy ability data to proj, snapshotted with damage and effects already rolled.
            ASSERT_OR_DIE(context.HasComponent<CProjectile>(projectileID), "ProjectileHitAbility::Update - spawned projectile is missing CProjectile component.");
            CProjectile& projComp = projectileStorage[projectileID];

            // copy targets from target chain to proj
			for (int chainIndex = 0; chainIndex < m_targetingComp.m_targetChains.GetDimensions().y; ++chainIndex)
			{
				EntityID chainTargetID = m_targetingComp.m_targetChains.Get(IntVec2(targetIndex, chainIndex));
				projComp.m_targets[chainIndex] = chainTargetID;
			}

            projComp.m_numChains = maxChains;
            projComp.m_targetPos = std::nullopt;
            projComp.m_accumulatedTime += m_cooldownComp.m_accumulatedTime;
            projComp.m_projSpeed = m_projSpeed * timeDilation;
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
void ProjectileHitAbility::CopyTransientDataTo(Ability& other) const
{
	ProjectileHitAbility& otherProj = static_cast<ProjectileHitAbility&>(other);

    otherProj.m_cooldownComp.m_accumulatedTime = m_cooldownComp.m_accumulatedTime;
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, Vec2 const& location) const
{
    float minRange = m_targetingComp.GetMinRange();
    float maxRange = m_targetingComp.GetMaxRange();

    if (minRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, minRange, 0.1f, 32, Rgba8::Green);
    }
    if (maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
	Ability::AppendDebugString(debugContext);
    debugContext.m_debugString += StringUtils::StringF("Proj Def: %s\n", m_projectileDefName.ToCStr());
    debugContext.m_debugString += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed * debugContext.m_entityTimeDilation);

    m_cooldownComp.AppendDebugString(debugContext);
    m_targetingComp.AppendDebugString(debugContext);
    m_critComp.AppendDebugString(debugContext);
    m_onHitComp.AppendDebugString(debugContext);
	m_multishotComp.AppendDebugString(debugContext);
	m_chainComp.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
RolledOnHitComponent ProjectileHitAbility::RollDamageAndEffects(RandomNumberGenerator& rng) const
{
	RolledOnHitComponent rolledHitResult;

	float critMultiplier = StaticGameSettings::s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.CanCrit())
    {
        float critRoll = rng.GetRandomFloatZeroToOne();
        critMultiplier += m_critComp.m_critMulti;
        didCrit = critRoll < m_critComp.m_critChance;
    }

    AbilityOnHitComponent const& onHitComp = m_onHitComp;
    HitPayload& rolledHitPayload = rolledHitResult.m_payload;
    rolledHitPayload.m_didCrit = didCrit;

    AbilityDamageComponent const& damageComp = onHitComp.m_damageOnHit;
    AbilityBurnComponent const& burnComp = onHitComp.m_burnOnHit;
    AbilityPoisonComponent const& poisonComp = onHitComp.m_poisonOnHit;
    AbilitySlowComponent const& slowComp = onHitComp.m_slowOnHit;

    rolledHitPayload.m_damage = rng.GetRandomFloatInRange(damageComp.GetMinDamage(), damageComp.GetMaxDamage());
    rolledHitPayload.m_burn = burnComp.GetBurn();
    rolledHitPayload.m_poison = poisonComp.GetPoison();
    rolledHitPayload.m_slowDuration = slowComp.GetDuration();

    if (didCrit)
    {
        rolledHitPayload.m_damage *= critMultiplier;
        rolledHitPayload.m_burn *= critMultiplier;
        rolledHitPayload.m_poison *= critMultiplier;
    }

    if (onHitComp.m_aoeHitOnHit.IsRelevant())
    {
        RolledAoEHitComponent& rolledAoeHitResult = rolledHitResult.m_aoeHitOnHit;
        rolledAoeHitResult.m_radius = onHitComp.m_aoeHitOnHit.GetRadius();
		HitPayload& rolledAoeHitPayload = rolledAoeHitResult.m_payload;
        rolledAoeHitPayload.m_didCrit = didCrit;

        AbilityAoEHitComponent const& aoeHitComp = onHitComp.m_aoeHitOnHit;
        if (aoeHitComp.m_damageOnHit.IsRelevant())
        {
            AbilityDamageComponent const& aoeDamageComp = aoeHitComp.m_damageOnHit;
            rolledAoeHitPayload.m_damage = rng.GetRandomFloatInRange(aoeDamageComp.GetMinDamage(), aoeDamageComp.GetMaxDamage());
        }

        if (aoeHitComp.m_burnOnHit.IsRelevant())
        {
            AbilityBurnComponent const& aoeBurnComp = aoeHitComp.m_burnOnHit;
            rolledAoeHitPayload.m_burn = aoeBurnComp.GetBurn();
        }

        if (aoeHitComp.m_poisonOnHit.IsRelevant())
        {
            AbilityPoisonComponent const& aoePoisonComp = aoeHitComp.m_poisonOnHit;
            rolledAoeHitPayload.m_poison = aoePoisonComp.GetPoison();
        }

        if (aoeHitComp.m_slowOnHit.IsRelevant())
        {
            AbilitySlowComponent const& aoeSlowComp = aoeHitComp.m_slowOnHit;
            rolledAoeHitPayload.m_slowDuration = aoeSlowComp.GetDuration();
		}

        if (didCrit)
        {
            rolledAoeHitPayload.m_damage *= critMultiplier;
            rolledAoeHitPayload.m_burn *= critMultiplier;
            rolledAoeHitPayload.m_poison *= critMultiplier;
        }
    }

    if (onHitComp.m_aoeEffectOnHit.IsRelevant())
    {
		rolledHitResult.m_aoeEffectOnHit = onHitComp.m_aoeEffectOnHit;
    }

	return rolledHitResult;
}



//----------------------------------------------------------------------------------------------------------------------
bool ProjectileHitAbility::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    bool applied = false;
    applied |= m_cooldownComp.ApplyModifier(modifier);
	applied |= m_targetingComp.ApplyModifier(modifier);
    applied |= m_critComp.ApplyModifier(modifier);
	applied |= m_chainComp.ApplyModifier(modifier);
	applied |= m_multishotComp.ApplyModifier(modifier);
	applied |= m_onHitComp.ApplyModifier(modifier);
    return applied;
}



//----------------------------------------------------------------------------------------------------------------------
AoEHitAbility::AoEHitAbility(AoEHitAbilityDef const& def) : Ability(def)
{
	m_cooldownComp = def.m_cooldownDef.has_value() ? AbilityCooldownComponent(def.m_cooldownDef.value()) : AbilityCooldownComponent();
	m_targetingComp = def.m_targetingDef.has_value() ? AbilityAoETargetingComponent(def.m_targetingDef.value()) : AbilityAoETargetingComponent();
	m_critComp = def.m_critDef.has_value() ? AbilityCritComponent(def.m_critDef.value()) : AbilityCritComponent();
	m_aoeHitComp = def.m_aoeHitDef.has_value() ? AbilityAoEHitComponent(def.m_aoeHitDef.value()) : AbilityAoEHitComponent();
	m_aoeEffectComp = def.m_aoeEffectDef.has_value() ? AbilityAoEEffectComponent(def.m_aoeEffectDef.value()) : AbilityAoEEffectComponent();
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation /*= 1.f*/)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    constexpr float maxAttacksPerSecond = 1000.f;
    constexpr float minTimeBetweenAttacks = 1.f / maxAttacksPerSecond;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown();
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, minTimeBetweenAttacks);

    if (m_cooldownComp.m_accumulatedTime <= timeBetweenAttacks)
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
	m_targetingComp.UpdateCachedTiles(context, location);

	float maxRange = m_targetingComp.GetMaxRange();

	if (!m_targetingComp.FindTargets(context))
    {
        // No targets in range, clamp cooldown
        m_cooldownComp.m_accumulatedTime = MathUtils::Clamp(m_cooldownComp.m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

    while (m_cooldownComp.m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp.m_accumulatedTime -= timeBetweenAttacks;

        if (m_aoeEffectComp.IsRelevant())
        {
            SpawnInfo aoeEffectSpawnInfo;
            aoeEffectSpawnInfo.m_spawnPos = location;
            aoeEffectSpawnInfo.m_spawnLifetime = m_aoeEffectComp.m_durationSeconds;
            aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_aoeEffectComp.m_aoeEffectDefName);
            aoeEffectSpawnInfo.m_spawnScale = maxRange; // Initial radius is assumed to be 1

            EntityID aoeEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);

            // Pass along damage, color, to aoe effect
            if (context.HasComponents(aoeEffect, collisionEffectBit))
            {
                CCollisionEffect& aoeEffectComp = collisionEffectStorage[aoeEffect];
                aoeEffectComp.InitializeFromAoEEffect(m_aoeEffectComp);
            }
		}

        HitPayload const& payload = RollDamageAndEffects(rng);
        if (!payload.HasValue())
        {
            continue;
        }

        for (EntityID entityID : m_targetingComp.m_targets)
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
                timeComp.m_remainingHasteDuration += payload.m_hasteDuration;
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
void AoEHitAbility::CopyTransientDataTo(Ability& other) const
{
	AoEHitAbility& otherAoE = static_cast<AoEHitAbility&>(other);

	otherAoE.m_cooldownComp.m_accumulatedTime = m_cooldownComp.m_accumulatedTime;
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, Vec2 const& location) const
{
    float minRange = m_targetingComp.GetMinRange();
    float maxRange = m_targetingComp.GetMaxRange();
    if (minRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, minRange, 0.1f, 32, Rgba8::Green);
    }
    if (maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

	m_cooldownComp.AppendDebugString(debugContext);
	m_targetingComp.AppendDebugString(debugContext);
	m_critComp.AppendDebugString(debugContext);
	m_aoeHitComp.AppendDebugString(debugContext);
	m_aoeEffectComp.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload AoEHitAbility::RollDamageAndEffects(RandomNumberGenerator& rng) const
{
	HitPayload payload;

    float critMultiplier = StaticGameSettings::s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.CanCrit())
    {
        AbilityCritComponent const& critComp = m_critComp;
        float critRoll = rng.GetRandomFloatZeroToOne();
        critMultiplier += critComp.m_critMulti;
        didCrit = critRoll < critComp.m_critChance;
		payload.m_didCrit = didCrit;
    }

    if (m_aoeHitComp.IsRelevant())
    {
        AbilityAoEHitComponent const& aoeHitComp = m_aoeHitComp;
        if (aoeHitComp.m_damageOnHit.IsRelevant())
        {
            AbilityDamageComponent const& damageComp = aoeHitComp.m_damageOnHit;
            payload.m_damage = rng.GetRandomFloatInRange(damageComp.GetMinDamage(), damageComp.GetMaxDamage());
            if (didCrit)
            {
                payload.m_damage *= critMultiplier;
            }
        }

        if (aoeHitComp.m_burnOnHit.IsRelevant())
        {
            AbilityBurnComponent const& burnComp = aoeHitComp.m_burnOnHit;
            payload.m_burn = burnComp.m_burn;
            if (didCrit)
            {
                payload.m_burn *= critMultiplier;
            }
        }

        if (aoeHitComp.m_poisonOnHit.IsRelevant())
        {
            AbilityPoisonComponent const& poisonComp = aoeHitComp.m_poisonOnHit;
            payload.m_poison = poisonComp.m_poison;
            if (didCrit)
            {
                payload.m_poison *= critMultiplier;
            }
        }

        if (aoeHitComp.m_slowOnHit.IsRelevant())
        {
            AbilitySlowComponent const& slowComp = aoeHitComp.m_slowOnHit;
            payload.m_slowDuration = slowComp.m_duration;
		}

        if (aoeHitComp.m_hasteOnHit.IsRelevant())
        {
			AbilityHasteComponent const& hasteComp = aoeHitComp.m_hasteOnHit;
			payload.m_hasteDuration = hasteComp.m_duration;
        }
    }

    return payload;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbility::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    bool applied = false;

    if (def.m_abilityAttribute == TowerAbilityAttribute::Range)
    {
        // For Passive Aoe abilities, range == AoE
        m_aoeEffectComp.m_radiusMultiplier += modifier.GetValue();
        applied = true;
    }

    if (def.m_abilityAttribute == TowerAbilityAttribute::AoE)
    {
        // For Passive Aoe abilities, range == AoE
        m_targetingComp.m_rangeMultiplier += modifier.GetValue();
        applied = true;
    }

	applied |= m_cooldownComp.ApplyModifier(modifier);
	applied |= m_targetingComp.ApplyModifier(modifier);
	applied |= m_critComp.ApplyModifier(modifier);
	applied |= m_aoeHitComp.ApplyModifier(modifier);
	applied |= m_aoeEffectComp.ApplyModifier(modifier);
    return applied;
}



//----------------------------------------------------------------------------------------------------------------------
PassiveAoEAbility::PassiveAoEAbility(PassiveAoEAbilityDef const& def) : Ability(def)
{
	m_targetingComp = def.m_targetingDef.has_value() ? *def.m_targetingDef : AbilityAoETargetingComponent();
	m_aoeEffectComp = def.m_aoeEffectDef.has_value() ? *def.m_aoeEffectDef : AbilityAoEEffectComponent();
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::Shutdown(SystemContext const& context)
{
	if (m_activeAoEEffect != EntityID::Invalid)
	{
		context.DestroyEntity(m_activeAoEEffect);
		m_activeAoEEffect = EntityID::Invalid;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "PassiveAoEAbility::Update - m_abilityDef is null.");

    // Write Dependencies
	auto& collisionEffectStorage = context.GetArrayStorage<CCollisionEffect>();
	auto& timeStorage = context.GetArrayStorage<CTime>();

	BitMask collisionEffectBit = context.GetComponentBitMask<CCollisionEffect>();

	float maxRange = m_targetingComp.GetMaxRange();

	if (m_needsEffectRespawn)
	{
		m_needsEffectRespawn = false;
		if (context.IsValid(m_activeAoEEffect))
		{
			context.DestroyEntity(m_activeAoEEffect);
			m_activeAoEEffect = EntityID::Invalid;
		}
	}

    if (!context.IsValid(m_activeAoEEffect))
    {
        SpawnInfo aoeEffectSpawnInfo;
        aoeEffectSpawnInfo.m_spawnPos = location;
        aoeEffectSpawnInfo.m_spawnLifetime = -1.f; // Infinite bc this is a passive ability
        aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_aoeEffectComp.m_aoeEffectDefName);
        aoeEffectSpawnInfo.m_spawnScale = maxRange;

        m_activeAoEEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);

        if (context.IsValid(m_activeAoEEffect))
        {
            // Pass along damage, color, to aoe effect
            if (context.HasComponents(m_activeAoEEffect, collisionEffectBit))
            {
				CCollisionEffect& aoeEffectComp = collisionEffectStorage[m_activeAoEEffect];
				aoeEffectComp.InitializeFromAoEEffect(m_aoeEffectComp);
            }
		}
    }

    if (context.IsValid(m_activeAoEEffect))
    {
        CTime& aoeEffectTimeComp = timeStorage[m_activeAoEEffect];
        aoeEffectTimeComp.m_clock.SetTimeDilation(timeDilation);
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
void PassiveAoEAbility::CopyTransientDataTo(Ability&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, Vec2 const& location) const
{
    float minRange = m_targetingComp.GetMinRange();
    float maxRange = m_targetingComp.GetMaxRange();
    if (minRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, minRange, 0.1f, 32, Rgba8::Green);
    }
    if (maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

	m_targetingComp.AppendDebugString(debugContext);
    m_aoeEffectComp.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
bool PassiveAoEAbility::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    bool applied = false;

	if (def.m_abilityAttribute == TowerAbilityAttribute::Range)
	{
        // For Passive Aoe abilities, range == AoE
		m_aoeEffectComp.m_radiusMultiplier += modifier.GetValue();
		applied = true;
	}

    if (def.m_abilityAttribute == TowerAbilityAttribute::AoE)
    {
        // For Passive Aoe abilities, range == AoE
        m_targetingComp.m_rangeMultiplier += modifier.GetValue();
        applied = true;
    }

	applied |= m_targetingComp.ApplyModifier(modifier);
    applied |= m_aoeEffectComp.ApplyModifier(modifier);

    m_needsEffectRespawn = applied;
    return applied;
}



//----------------------------------------------------------------------------------------------------------------------
AdjacentHitAbility::AdjacentHitAbility(AdjacentHitAbilityDef const& def) : Ability(def)
{
	m_cooldownComp = def.m_cooldownDef.has_value() ? *def.m_cooldownDef : AbilityCooldownComponent();
	m_hasteOnHit = def.m_hasteOnHit.has_value() ? *def.m_hasteOnHit : AbilityHasteComponent();
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbility::Update(SystemContext const& context, Vec2 const&, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "AdjacentHitAbility::Update - m_abilityDef is null.");

    float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    constexpr float maxAttacksPerSecond = 1000.f;
    constexpr float minTimeBetweenAttacks = 1.f / maxAttacksPerSecond;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown();
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, minTimeBetweenAttacks);

    if (m_cooldownComp.m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

    if (!m_targetingComp.FindTargets(context, m_owner))
    {
        // No targets in range, clamp cooldown
        m_cooldownComp.m_accumulatedTime = MathUtils::Clamp(m_cooldownComp.m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

	auto& timeStorage = context.GetArrayStorage<CTime>();

    while (m_cooldownComp.m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp.m_accumulatedTime -= timeBetweenAttacks;

		for (EntityID targetID : m_targetingComp.m_targets)
		{
			if (!context.IsValid(targetID))
			{
				continue;
			}

            if (m_hasteOnHit.IsRelevant())
            {
                CTime& time = timeStorage[targetID];
                time.m_remainingHasteDuration += m_hasteOnHit.GetDuration();
            }
		}
    }
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AdjacentHitAbility::DeepCopy() const
{
	return new AdjacentHitAbility(*reinterpret_cast<AdjacentHitAbilityDef const*>(m_abilityDef));
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbility::CopyTransientDataTo(Ability& other) const
{
    AdjacentHitAbility& otherAbility = static_cast<AdjacentHitAbility&>(other);

    otherAbility.m_cooldownComp.m_accumulatedTime = m_cooldownComp.m_accumulatedTime;
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbility::AddDebugVerts(VertexBuffer& vbo, CPlaceable const& placeable, Vec2 const&) const
{
	IntVec2 topRight = placeable.m_botLeftTile + placeable.m_dims - IntVec2::OneVector;

	Rgba8 tint = Rgba8::Orange;
	tint.a = 128;

    // Top Row
	for (int x = placeable.m_botLeftTile.x; x <= topRight.x; ++x)
	{
		AABB2 aabb = AABB2(Vec2((float) x, (float) topRight.y + 1.f), Vec2((float) x + 1.f, (float) topRight.y + 2.f));
		aabb.Translate(Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY));
		VertexUtils::AddVertsForAABB2(vbo, aabb, tint);
	}

	// Bottom Row
	for (int x = placeable.m_botLeftTile.x; x <= topRight.x; ++x)
	{
		AABB2 aabb = AABB2(Vec2((float) x, (float) placeable.m_botLeftTile.y - 1.f), Vec2((float) x + 1.f, (float) placeable.m_botLeftTile.y));
		aabb.Translate(Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY));
		VertexUtils::AddVertsForAABB2(vbo, aabb, tint);
	}

    // Left Column
	for (int y = placeable.m_botLeftTile.y; y <= topRight.y; ++y)
	{
		AABB2 aabb = AABB2(Vec2((float) placeable.m_botLeftTile.x - 1.f, (float) y), Vec2((float) placeable.m_botLeftTile.x, (float) y + 1.f));
		aabb.Translate(Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY));
		VertexUtils::AddVertsForAABB2(vbo, aabb, tint);
	}

    // Right Column
	for (int y = placeable.m_botLeftTile.y; y <= topRight.y; ++y)
	{
		AABB2 aabb = AABB2(Vec2((float) topRight.x + 1.f, (float) y), Vec2((float) topRight.x + 2.f, (float) y + 1.f));
		aabb.Translate(Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY));
		VertexUtils::AddVertsForAABB2(vbo, aabb, tint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

	m_cooldownComp.AppendDebugString(debugContext);
	m_hasteOnHit.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdjacentHitAbility::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	m_cooldownComp.ApplyModifier(modifier);
	m_hasteOnHit.ApplyModifier(modifier);

    // Nothing can affect range of "adjacent", so dont apply mods to targeting component

    return false;
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
    m_targetingComp = def.m_targetingDef.has_value() ? *def.m_targetingDef : AbilityPrecisionTargetingComponent();
	m_onHitComp = def.m_onHitDef.has_value() ? *def.m_onHitDef : AbilityOnHitComponent();
	m_renderComp = def.m_renderDef.has_value() ? *def.m_renderDef : AbilityRenderComponent();
	m_chainComp = def.m_chainDef.has_value() ? *def.m_chainDef : AbilityChainComponent();
	m_multishotComp = def.m_multishotDef.has_value() ? *def.m_multishotDef : AbilityMultishotComponent();
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation)
{
	ASSERT_OR_DIE(m_abilityDef, "LaserAbility::Update - m_abilityDef is null.");

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    if (deltaSeconds == 0.f)
    {
        return;
    }

    BitMask healthBit = context.GetComponentBitMask<CHealth>();
    BitMask timeBit = context.GetComponentBitMask<CTime>();

    // Cache tiles in range as optimization, so we never search non path tiles that are out of range
    m_targetingComp.UpdateCachedTiles(context, location);

    int maxTargets = m_multishotComp.m_additionalTargets + 1;
    int maxChains = m_chainComp.m_maxChains;
    float chainDistance = m_chainComp.m_chainDistance;

    if (!m_targetingComp.FindTargets(context, maxTargets, maxChains, chainDistance))
    {
        return;
    }

    HitPayload const payload = RollDamageAndEffects(deltaSeconds);

    for (int targetIndex = 0; targetIndex < m_targetingComp.m_targetChains.GetDimensions().x; ++targetIndex)
    {
        float chainPayloadMulti = 1.f;

        for (int chainIndex = 0; chainIndex < m_targetingComp.m_targetChains.GetDimensions().y; ++chainIndex)
        {
            EntityID target = m_targetingComp.m_targetChains.Get(IntVec2(targetIndex, chainIndex));
            if (target == EntityID::Invalid)
            {
                break;
            }

			HitPayload chainPayload = payload;
            chainPayload *= chainPayloadMulti;

            chainPayloadMulti *= m_chainComp.m_chainPayloadMulti;

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

            if (m_onHitComp.m_aoeEffectOnHit.IsRelevant())
            {
                CTransform const& targetTransform = *context.GetComponentConst<CTransform>(target);

                SpawnInfo aoeEffectSpawnInfo;
                aoeEffectSpawnInfo.m_spawnPos = targetTransform.m_pos;
                aoeEffectSpawnInfo.m_spawnLifetime = m_onHitComp.m_aoeEffectOnHit.GetDuration();
                aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_onHitComp.m_aoeEffectOnHit.m_aoeEffectDefName);
                aoeEffectSpawnInfo.m_spawnScale = m_onHitComp.m_aoeEffectOnHit.GetRadius();

                EntityID aoeEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);
                if (context.IsValid(aoeEffect))
                {
                    CCollisionEffect& aoeEffectComp = *context.GetComponent<CCollisionEffect>(aoeEffect);
                    aoeEffectComp.InitializeFromAoEEffect(m_onHitComp.m_aoeEffectOnHit);
                }
            }
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Render(SystemContext const& context, Vec2 const& location) const
{
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    VertexBuffer& vbo = *renderer.GetVertexBuffer(scRenderer.m_immediateVBO);
    vbo.ClearVerts();

    for (int targetIndex = 0; targetIndex < m_targetingComp.m_targetChains.GetDimensions().x; ++targetIndex)
    {
        Vec2 currentChainStartLocation = location;

        for (int chainIndex = 0; chainIndex < m_targetingComp.m_targetChains.GetDimensions().y; ++chainIndex)
        {
            EntityID target = m_targetingComp.m_targetChains.Get(IntVec2(targetIndex, chainIndex));
            if (!context.IsValid(target))
            {
                continue;
            }

            if (CTransform const* transform = context.GetComponent<CTransform>(target))
            {
                VertexUtils::AddVertsForLine2D(vbo, currentChainStartLocation, transform->m_pos, 0.25f, m_renderComp.m_tint, m_renderComp.m_depth);
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
void LaserAbility::CopyTransientDataTo(Ability&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, Vec2 const& location) const
{
    float minRange = m_targetingComp.GetMinRange();
    float maxRange = m_targetingComp.GetMaxRange();
    if (minRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, minRange, 0.1f, 32, Rgba8::Green);
    }
    if (maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

    m_targetingComp.AppendDebugString(debugContext);    
    m_onHitComp.AppendDebugString(debugContext);
	m_chainComp.AppendDebugString(debugContext);
	m_multishotComp.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload LaserAbility::RollDamageAndEffects(float deltaSeconds) const
{
    HitPayload result;
    result.m_didCrit = false;
	result.m_damage = m_onHitComp.m_damageOnHit.GetMaxDamage();
    result.m_burn = m_onHitComp.m_burnOnHit.GetBurn();
    result.m_poison = m_onHitComp.m_poisonOnHit.GetPoison();
    result.m_slowDuration = m_onHitComp.m_slowOnHit.GetDuration();
    result *= deltaSeconds;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
bool LaserAbility::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	bool didApply = false;
	didApply |= m_targetingComp.ApplyModifier(modifier);
	didApply |= m_onHitComp.ApplyModifier(modifier);
	didApply |= m_chainComp.ApplyModifier(modifier);
	didApply |= m_multishotComp.ApplyModifier(modifier);
    return didApply;
}
