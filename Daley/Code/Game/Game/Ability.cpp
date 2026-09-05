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
void AbilityTargetingComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();
	if (def.m_abilityAttribute == TowerAbilityAttribute::Range)
	{
		m_rangeMultiplier += modifier.GetValue();
		m_needsCacheUpdate = 1;
	}
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
                if (distSquared > rangeSquared || distSquared < rangeSquared)
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
				if (distSquared > rangeSquared || distSquared < rangeSquared)
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
				break;
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
void AbilityCooldownComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();
    if (def.m_abilityAttribute == TowerAbilityAttribute::AttackSpeed)
    {
		m_attackSpeedIncrease += modifier.GetValue();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float cooldown = GetCooldown() / debugContext.m_entityTimeDilation;
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
void AbilityCritComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();
    if (def.m_abilityAttribute == TowerAbilityAttribute::CritChance)
    {
		m_critChance += modifier.GetValue();
    }
    else if (def.m_abilityAttribute == TowerAbilityAttribute::CritDamage)
    {
		m_critMulti += modifier.GetValue();
    }
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
void AbilityDamageComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::Damage)
	{
		m_damageMultiplier += modifier.GetValue();
	}
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
void AbilityBurnComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    if (def.m_abilityAttribute == TowerAbilityAttribute::Burn)
    {
        m_burnMultiplier += modifier.GetValue();
    }
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
void AbilityPoisonComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    if (def.m_abilityAttribute == TowerAbilityAttribute::Poison)
    {
        m_poisonMultiplier += modifier.GetValue();
    }
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
void AbilitySlowComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

    if (def.m_abilityAttribute == TowerAbilityAttribute::Slow)
    {
        m_durationMultiplier += modifier.GetValue();
    }
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
void AbilityHasteComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::Haste)
	{
		m_durationMultiplier += modifier.GetValue();
	}
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
void AbilityChainComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::NumChains)
	{
		int chainIncrease = MathUtils::RoundF(modifier.GetValue());
		m_maxChains += chainIncrease;
	}
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
void AbilityMultishotComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	TowerAbilityRunModifierDef const& def = modifier.GetDef();

	if (def.m_abilityAttribute == TowerAbilityAttribute::Multishot)
	{
		int additionalTargetsIncrease = MathUtils::RoundF(modifier.GetValue());
		m_additionalTargets += additionalTargetsIncrease;
	}
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
void AbilityOnHitComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	m_damageOnHit.ApplyModifier(modifier);
	m_burnOnHit.ApplyModifier(modifier);
	m_poisonOnHit.ApplyModifier(modifier);
	m_slowOnHit.ApplyModifier(modifier);
	m_aoeHitOnHit.ApplyModifier(modifier);
	m_aoeEffectOnHit.ApplyModifier(modifier);
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
	bool hasRadius = m_radius > 0.f;
	if (!hasRadius)
	{
		return false;
	}

	bool hasPayload = m_damageOnHit.IsRelevant() || m_poisonOnHit.IsRelevant() || m_burnOnHit.IsRelevant() || m_slowOnHit.IsRelevant() || m_hasteOnHit.IsRelevant();
    return hasPayload;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    m_damageOnHit.ApplyModifier(modifier);
	m_burnOnHit.ApplyModifier(modifier);
	m_poisonOnHit.ApplyModifier(modifier);
	m_slowOnHit.ApplyModifier(modifier);
	m_hasteOnHit.ApplyModifier(modifier);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (!IsRelevant())
    {
		return;
	}   

    debugContext.m_debugString += StringUtils::StringF("---AOE Hit---\n");
    if (m_radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", m_radius);
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

	if (m_radius <= 0.f)
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

    debugContext.m_debugString += StringUtils::StringF("---AOE Effect---\n", m_radius);
    if (m_radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", m_radius);
	}
    debugContext.m_debugString += StringUtils::StringF("Duration: %.1f\n", m_durationSeconds);	

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
void AbilityAoEEffectComponent::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	m_damagePerSecond.ApplyModifier(modifier);
	m_burnPerSecond.ApplyModifier(modifier);
	m_poisonPerSecond.ApplyModifier(modifier);
	m_slowPerSecond.ApplyModifier(modifier);
	m_hastePerSecond.ApplyModifier(modifier);
}



//----------------------------------------------------------------------------------------------------------------------
Ability::Ability(AbilityDef const& def) : m_abilityDef(&def)
{
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
void Ability::ApplyModifier(TowerAbilityRunModifier const&)
{

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

    // Shoot at targets
    while (m_cooldownComp.m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp.m_accumulatedTime -= timeBetweenAttacks;

        for (int targetIndex = 0; targetIndex < m_targetingComp.m_targetChains.GetDimensions().x; ++targetIndex)
        {
            EntityID targetID = m_targetingComp.m_targetChains.Get(IntVec2(targetIndex, 0));

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
void ProjectileHitAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
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
	RolledOnHitComponent hitResult;

	float critMultiplier = StaticGameSettings::s_baseCritMultiplier;
    bool didCrit = false;
    if (m_critComp.CanCrit())
    {
        float critRoll = rng.GetRandomFloatZeroToOne();
        critMultiplier += m_critComp.m_critMulti;
        didCrit = critRoll < m_critComp.m_critChance;
    }

    AbilityOnHitComponent const& onHitComp = m_onHitComp;
    HitPayload& onHitPayload = hitResult.m_payload;
    onHitPayload.m_didCrit = didCrit;

    AbilityDamageComponent const& damageComp = onHitComp.m_damageOnHit;
    onHitPayload.m_damage = rng.GetRandomFloatInRange(damageComp.GetMinDamage(), damageComp.GetMaxDamage());
    if (didCrit)
    {
        onHitPayload.m_damage *= critMultiplier;
    }

    AbilityBurnComponent const& burnComp = onHitComp.m_burnOnHit;
    onHitPayload.m_burn = burnComp.GetBurn();
    if (didCrit)
    {
        onHitPayload.m_burn *= critMultiplier;
    }

    AbilityPoisonComponent const& poisonComp = onHitComp.m_poisonOnHit;
    onHitPayload.m_poison = poisonComp.GetPoison();
    if (didCrit)
    {
        onHitPayload.m_poison *= critMultiplier;
    }

    AbilitySlowComponent const& slowComp = onHitComp.m_slowOnHit;
    onHitPayload.m_slowDuration = slowComp.GetDuration();

    if (onHitComp.m_aoeHitOnHit.IsRelevant())
    {
        RolledAoEHitComponent aoeHitResult;
        aoeHitResult.m_radius = onHitComp.m_aoeHitOnHit.m_radius;
		HitPayload& aoeHitPayload = aoeHitResult.m_payload;
        aoeHitPayload.m_didCrit = didCrit;

        AbilityAoEHitComponent const& aoeHitComp = onHitComp.m_aoeHitOnHit;
        if (aoeHitComp.m_damageOnHit.IsRelevant())
        {
            AbilityDamageComponent const& aoeDamageComp = aoeHitComp.m_damageOnHit;
            aoeHitPayload.m_damage = rng.GetRandomFloatInRange(aoeDamageComp.GetMinDamage(), aoeDamageComp.GetMaxDamage());
            if (didCrit)
            {
                aoeHitPayload.m_damage *= critMultiplier;
            }
        }

        if (aoeHitComp.m_burnOnHit.IsRelevant())
        {
            AbilityBurnComponent const& aoeBurnComp = aoeHitComp.m_burnOnHit;
            aoeHitPayload.m_burn = aoeBurnComp.GetBurn();
            if (didCrit)
            {
                aoeHitPayload.m_burn *= critMultiplier;
            }
        }

        if (aoeHitComp.m_poisonOnHit.IsRelevant())
        {
            AbilityPoisonComponent const& aoePoisonComp = aoeHitComp.m_poisonOnHit;
            aoeHitPayload.m_poison = aoePoisonComp.GetPoison();
            if (didCrit)
            {
                aoeHitPayload.m_poison *= critMultiplier;
            }
        }

        if (aoeHitComp.m_slowOnHit.IsRelevant())
        {
            AbilitySlowComponent const& aoeSlowComp = aoeHitComp.m_slowOnHit;
            aoeHitPayload.m_slowDuration = aoeSlowComp.GetDuration();
		}

		hitResult.m_aoeHitOnHit = aoeHitResult;
    }

    if (onHitComp.m_aoeEffectOnHit.IsRelevant())
    {
		hitResult.m_aoeEffectOnHit = onHitComp.m_aoeEffectOnHit;
    }

	return hitResult;
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbility::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
    m_cooldownComp.ApplyModifier(modifier);
	m_targetingComp.ApplyModifier(modifier);
    m_critComp.ApplyModifier(modifier);
	m_chainComp.ApplyModifier(modifier);
	m_multishotComp.ApplyModifier(modifier);
	m_onHitComp.ApplyModifier(modifier);
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
void AoEHitAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
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

	BitMask collisionEffectBit = context.GetComponentBitMask<CCollisionEffect>();

	float maxRange = m_targetingComp.GetMaxRange();

    if (m_activeAoEEffect == EntityID::Invalid)
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
    else
    {
		CTime* aoeEffectTimeComp = context.GetComponent<CTime>(m_activeAoEEffect);
        if (aoeEffectTimeComp)
        {
			aoeEffectTimeComp->m_clock.SetTimeDilation(timeDilation);
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
void PassiveAoEAbility::CopyTransientDataTo(Ability&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
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
void PassiveAoEAbility::ApplyModifier(TowerAbilityRunModifier const& modifier)
{
	m_targetingComp.ApplyModifier(modifier);
    m_aoeEffectComp.ApplyModifier(modifier);
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
                aoeEffectSpawnInfo.m_spawnLifetime = m_onHitComp.m_aoeEffectOnHit.m_durationSeconds;
                aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_onHitComp.m_aoeEffectOnHit.m_aoeEffectDefName);
                aoeEffectSpawnInfo.m_spawnScale = m_onHitComp.m_aoeEffectOnHit.m_radius;

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
void LaserAbility::AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const
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
