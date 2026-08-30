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
static constexpr float s_baseCritMultiplier = 2.f; // todo: move to gameplay static constants file?



//----------------------------------------------------------------------------------------------------------------------
AbilityTargetingComponent::AbilityTargetingComponent(AbilityTargetingComponentDef const& def)
{
    m_minRange = def.m_minRange;
	m_maxRange = def.m_maxRange;
	m_abilityTargetFlags = def.m_abilityTargetFlags;
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

        m_cachedTilesInRange.clear();

        // Note: Tower case covers Tower+Enemy case, but Enemy case does not. So we check tower first.
        if (m_abilityTargetFlags & (uint8_t) AbilityTargetFlags::Tower)
        {
            world.ForEachPlayableTileOverlappingCircle(location, m_maxRange, [&](IntVec2 const& worldCoords)
            {
                m_cachedTilesInRange.push_back(worldCoords);
                return true;
            });
        }
        else if (m_abilityTargetFlags & (uint8_t) AbilityTargetFlags::Enemy)
        {
            world.ForEachPathTileInRange(location, m_minRange, m_maxRange, [&](IntVec2 const& worldCoords)
            {
                m_cachedTilesInRange.push_back(worldCoords);
                return true;
            });
        }

        m_minRangeAtTimeOfCache = m_minRange;
        m_maxRangeAtTimeOfCache = m_maxRange;
        m_locationAtTimeOfCache = location;

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
    return needsUpdate;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityTargetingComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (m_minRange > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Range: %.1f - %.1f\n", m_minRange, m_maxRange);
    }
    else
    {
        debugContext.m_debugString += StringUtils::StringF("Range: %.1f\n", m_maxRange);
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
	CollisionLayer const& buildingLayer = collision.GetCollisionLayer(CollisionChannel::Building);
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	auto& transformStorage = context.GetArrayStorageConst<CTransform>();
	BitMask healthBit = context.GetComponentBitMask<CHealth>();

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

				CTransform const& pos = transformStorage[entityID];
				float distSquared = MathUtils::GetDistanceSquared2D(pos.m_pos, m_locationAtTimeOfCache);
				if (distSquared > m_maxRange * m_maxRange || distSquared < m_minRange * m_minRange)
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

    CollisionLayer const& enemyLayer = collision.GetCollisionLayer(CollisionChannel::Enemy);

    float maxDistanceSquared = maxDistance * maxDistance;

    EntityID result = EntityID::Invalid;

    for (auto& tile : m_cachedTilesInRange)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(tile);
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
                break;
            }
        }
    }

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCooldownComponent::AbilityCooldownComponent(AbilityCooldownComponentDef const& def)
{
	m_cooldownSeconds = def.m_cooldownSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float cooldown = m_cooldownSeconds / debugContext.m_timeDilation;
    debugContext.m_debugString += StringUtils::StringF("Cooldown: %.3f\n", cooldown);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCritComponent::AbilityCritComponent(AbilityCritComponentDef const& def)
{
    m_critChance = def.m_critChance;
	m_critMulti = def.m_critMulti;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCritComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    if (m_critChance <= 0.f)
    {
        return;
	}
    debugContext.m_debugString += StringUtils::StringF("Crit Chance: %.1f%%\n", m_critChance * 100.f);
    debugContext.m_debugString += StringUtils::StringF("Crit Mult: %.1f\n", 2.f + m_critMulti);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDamageComponent::AbilityDamageComponent(AbilityDamageComponentDef const& def)
{
    m_minDamage = def.m_minDamage;
	m_maxDamage = def.m_maxDamage;
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
void AbilityChainComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    debugContext.m_debugString += StringUtils::StringF("Chain Chance: %.2f\n", m_chainChance * 100.f);
    debugContext.m_debugString += StringUtils::StringF("Max Chains: %d\n", m_maxChains);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityMultishotComponent::AbilityMultishotComponent(AbilityMultishotComponentDef const& def)
{
	m_additionalTargets = def.m_additionalTargets;
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
    m_damageOnHit = def.m_damageOnHit;
	m_poisonOnHit = def.m_poisonOnHit;
	m_burnOnHit = def.m_burnOnHit;
	m_aoeHitOnHit = def.m_aoeHitOnHit;
	m_aoeEffectOnHit = def.m_aoeEffectOnHit;
	m_slowOnHit = def.m_slowOnHit;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityOnHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    debugContext.m_debugString += StringUtils::StringF("---Hit---\n");

	if (m_damageOnHit.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF("D(%.1f-%.1f)", m_damageOnHit->GetMinDamage(), m_damageOnHit->GetMaxDamage());
	}

	if (m_burnOnHit.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF(" B(%.1f)", m_burnOnHit->GetBurn());
	}

	if (m_poisonOnHit.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF(" P(%.1f)", m_poisonOnHit->GetPoison());
	}

	if (m_slowOnHit.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF(" S(%.1f)", m_slowOnHit->GetDuration());
	}
	debugContext.m_debugString += '\n';

    if (m_aoeHitOnHit.has_value())
    {
        m_aoeHitOnHit->AppendDebugString(debugContext);
    }
    if (m_aoeEffectOnHit.has_value())
    {
        m_aoeEffectOnHit->AppendDebugString(debugContext);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityOnHitComponent::ApplyModifier(TowerPayloadRunModifier const& modifier)
{
	TowerPayloadRunModifierDef const& def = modifier.GetDef();

    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Damage)
    {
        if (m_damageOnHit.has_value())
        {
            m_damageOnHit->m_damageMultiplier += def.m_multiplierIncreaseBase;
            m_damageOnHit->m_damageMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Burn)
    {
        if (m_burnOnHit.has_value())
        {
            m_burnOnHit->m_burnMultiplier += def.m_multiplierIncreaseBase;
            m_burnOnHit->m_burnMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Poison)
    {
        if (m_poisonOnHit.has_value())
        {
            m_poisonOnHit->m_poisonMultiplier += def.m_multiplierIncreaseBase;
            m_poisonOnHit->m_poisonMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Slow)
    {
        if (m_slowOnHit.has_value())
        {
            m_slowOnHit->m_durationMultiplier += def.m_multiplierIncreaseBase;
            m_slowOnHit->m_durationMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (m_aoeHitOnHit.has_value())
    {
        m_aoeHitOnHit->ApplyModifier(modifier);
    }
    if (m_aoeEffectOnHit.has_value())
    {
        m_aoeEffectOnHit->ApplyModifier(modifier);
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
	m_hasteOnHit = def.m_hasteOnHit;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    debugContext.m_debugString += StringUtils::StringF("---AOE Hit---\n");
    if (m_radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", m_radius);
    }

    if (m_damageOnHit.has_value())
    {
		float minDamage = m_damageOnHit->GetMinDamage();
		float maxDamage = m_damageOnHit->GetMaxDamage();
		debugContext.m_debugString += StringUtils::StringF("D(%.1f-%.1f) ", minDamage, maxDamage);
    }

	if (m_poisonOnHit.has_value())
	{
		float poison = m_poisonOnHit->GetPoison();
		debugContext.m_debugString += StringUtils::StringF("P(%.1f) ", poison);
	}

	if (m_burnOnHit.has_value())
	{
		float burn = m_burnOnHit->GetBurn();
		debugContext.m_debugString += StringUtils::StringF("B(%.1f) ", burn);
	}

	if (m_slowOnHit.has_value())
	{
		float slow = m_slowOnHit->GetDuration();
		debugContext.m_debugString += StringUtils::StringF("S(%.1f) ", slow);
	}

	if (m_hasteOnHit.has_value())
	{
		float haste = m_hasteOnHit->GetDuration();
		debugContext.m_debugString += StringUtils::StringF("H(%.1f)", haste);
	}

	debugContext.m_debugString += '\n';
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponent::ApplyModifier(TowerPayloadRunModifier const& modifier)
{
    TowerPayloadRunModifierDef const& def = modifier.GetDef();

    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Damage)
    {
        if (m_damageOnHit.has_value())
        {
            m_damageOnHit->m_damageMultiplier += def.m_multiplierIncreaseBase;
            m_damageOnHit->m_damageMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Burn)
    {
        if (m_burnOnHit.has_value())
        {
            m_burnOnHit->m_burnMultiplier += def.m_multiplierIncreaseBase;
            m_burnOnHit->m_burnMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Poison)
    {
        if (m_poisonOnHit.has_value())
        {
            m_poisonOnHit->m_poisonMultiplier += def.m_multiplierIncreaseBase;
            m_poisonOnHit->m_poisonMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Slow)
    {
        if (m_slowOnHit.has_value())
        {
            m_slowOnHit->m_durationMultiplier += def.m_multiplierIncreaseBase;
            m_slowOnHit->m_durationMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
	if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Haste)
	{
		if (m_hasteOnHit.has_value())
		{
			m_hasteOnHit->m_durationMultiplier += def.m_multiplierIncreaseBase;
			m_hasteOnHit->m_durationMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
		}
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
void AbilityAoEEffectComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
    debugContext.m_debugString += StringUtils::StringF("---AOE Effect---\n", m_radius);
    if (m_radius > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Radius: %.1f\n", m_radius);
	}
    debugContext.m_debugString += StringUtils::StringF("Duration: %.1f\n", m_durationSeconds);	

	if (m_damagePerSecond.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF("DPS(%.1f)", m_damagePerSecond->GetMaxDamage() * debugContext.m_timeDilation);
	}
	if (m_burnPerSecond.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF(" BPS(%.1f)", m_burnPerSecond->GetBurn() * debugContext.m_timeDilation);
	}
	if (m_poisonPerSecond.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF(" PPS(%.1f)", m_poisonPerSecond->GetPoison() * debugContext.m_timeDilation);
	}
	if (m_slowPerSecond.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF(" SPS(%.1f)", m_slowPerSecond->GetDuration() * debugContext.m_timeDilation);
	}
	if (m_hastePerSecond.has_value())
	{
		debugContext.m_debugString += StringUtils::StringF(" HPS(%.1f)", m_hastePerSecond->GetDuration() * debugContext.m_timeDilation);
	}
	debugContext.m_debugString += '\n';
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEEffectComponent::ApplyModifier(TowerPayloadRunModifier const& modifier)
{
    TowerPayloadRunModifierDef const& def = modifier.GetDef();

    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Damage)
    {
        if (m_damagePerSecond.has_value())
        {
            m_damagePerSecond->m_damageMultiplier += def.m_multiplierIncreaseBase;
            m_damagePerSecond->m_damageMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Burn)
    {
        if (m_burnPerSecond.has_value())
        {
            m_burnPerSecond->m_burnMultiplier += def.m_multiplierIncreaseBase;
            m_burnPerSecond->m_burnMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Poison)
    {
        if (m_poisonPerSecond.has_value())
        {
            m_poisonPerSecond->m_poisonMultiplier += def.m_multiplierIncreaseBase;
            m_poisonPerSecond->m_poisonMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Slow)
    {
        if (m_slowPerSecond.has_value())
        {
            m_slowPerSecond->m_durationMultiplier += def.m_multiplierIncreaseBase;
            m_slowPerSecond->m_durationMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
    if (def.m_payloadDamageTypeFlags & (uint8_t) PayloadType::Haste)
    {
        if (m_hastePerSecond.has_value())
        {
            m_hastePerSecond->m_durationMultiplier += def.m_multiplierIncreaseBase;
            m_hastePerSecond->m_durationMultiplier += def.m_multiplierIncreasePerLevel * (modifier.m_level - 1);
        }
    }
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
void Ability::ApplyModifier(TowerPayloadRunModifier const&, CTags const&)
{

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
void ProjectileHitAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");
    ASSERT_OR_DIE(m_cooldownComp.has_value(), "ProjectileHitAbility::Update - m_cooldownComp is null.");
    ASSERT_OR_DIE(m_targetingComp.has_value(), "ProjectileHitAbility::Update - m_targetingComp is null.");
    ASSERT_OR_DIE(m_onHitComp.has_value(), "ProjectileHitAbility::Update - m_onHitComp is null.");

    float deltaSeconds = context.m_deltaSeconds * timeDilation;
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

    if (m_cooldownComp.has_value() && otherProj.m_cooldownComp.has_value())
    {
        otherProj.m_cooldownComp->m_accumulatedTime = m_cooldownComp->m_accumulatedTime;
    }
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
void ProjectileHitAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
	Ability::AppendDebugString(debugContext);
    debugContext.m_debugString += StringUtils::StringF("Proj Def: %s\n", m_projectileDefName.ToCStr());
    debugContext.m_debugString += StringUtils::StringF("Proj Speed: %.1f\n", m_projSpeed * debugContext.m_timeDilation);

    if (m_cooldownComp.has_value())     
    {
        m_cooldownComp->AppendDebugString(debugContext);
    }
    if (m_targetingComp.has_value())
    {
        m_targetingComp->AppendDebugString(debugContext);
    }
    if (m_critComp.has_value())
    {
        m_critComp->AppendDebugString(debugContext);
    }
    if (m_onHitComp.has_value())
    {
        m_onHitComp->AppendDebugString(debugContext);
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
            onHitPayload.m_damage = rng.GetRandomFloatInRange(damageComp.GetMinDamage(), damageComp.GetMaxDamage());
            if (didCrit)
            {
                onHitPayload.m_damage *= critMultiplier;
            }
        }

        if (onHitComp.m_burnOnHit.has_value())
        {
            AbilityBurnComponent const& burnComp = onHitComp.m_burnOnHit.value();
            onHitPayload.m_burn = burnComp.GetBurn();
            if (didCrit)
            {
                onHitPayload.m_burn *= critMultiplier;
            }
        }

        if (onHitComp.m_poisonOnHit.has_value())
        {
            AbilityPoisonComponent const& poisonComp = onHitComp.m_poisonOnHit.value();
            onHitPayload.m_poison = poisonComp.GetPoison();
            if (didCrit)
            {
                onHitPayload.m_poison *= critMultiplier;
            }
        }

        if (onHitComp.m_slowOnHit.has_value())
        {
            AbilitySlowComponent const& slowComp = onHitComp.m_slowOnHit.value();
            onHitPayload.m_slowDuration = slowComp.GetDuration();
		}

        if (onHitComp.m_aoeHitOnHit.has_value())
        {
            RolledAoEHitComponent aoeHitResult;
            aoeHitResult.m_radius = onHitComp.m_aoeHitOnHit->m_radius;
			HitPayload& aoeHitPayload = aoeHitResult.m_payload;
            aoeHitPayload.m_didCrit = didCrit;

            AbilityAoEHitComponent const& aoeHitComp = onHitComp.m_aoeHitOnHit.value();
            if (aoeHitComp.m_damageOnHit.has_value())
            {
                AbilityDamageComponent const& aoeDamageComp = aoeHitComp.m_damageOnHit.value();
                aoeHitPayload.m_damage = rng.GetRandomFloatInRange(aoeDamageComp.GetMinDamage(), aoeDamageComp.GetMaxDamage());
                if (didCrit)
                {
                    aoeHitPayload.m_damage *= critMultiplier;
                }
            }

            if (aoeHitComp.m_burnOnHit.has_value())
            {
                AbilityBurnComponent const& aoeBurnComp = aoeHitComp.m_burnOnHit.value();
                aoeHitPayload.m_burn = aoeBurnComp.GetBurn();
                if (didCrit)
                {
                    aoeHitPayload.m_burn *= critMultiplier;
                }
            }

            if (aoeHitComp.m_poisonOnHit.has_value())
            {
                AbilityPoisonComponent const& aoePoisonComp = aoeHitComp.m_poisonOnHit.value();
                aoeHitPayload.m_poison = aoePoisonComp.GetPoison();
                if (didCrit)
                {
                    aoeHitPayload.m_poison *= critMultiplier;
                }
            }

            if (aoeHitComp.m_slowOnHit.has_value())
            {
                AbilitySlowComponent const& aoeSlowComp = aoeHitComp.m_slowOnHit.value();
                aoeHitPayload.m_slowDuration = aoeSlowComp.GetDuration();
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
void ProjectileHitAbility::ApplyModifier(TowerPayloadRunModifier const& modifier, CTags const& tags)
{
    for (auto& tag : modifier.GetDef().m_tagRequirements)
    {
        if (!tags.HasTag(tag))
        {
            return;
        }
    }

	if (m_onHitComp.has_value())
	{
		m_onHitComp->ApplyModifier(modifier);
	}
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
void AoEHitAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation /*= 1.f*/)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");
    ASSERT_OR_DIE(m_cooldownComp.has_value(), "ProjectileHitAbility::Update - m_cooldownComp is null.");
    ASSERT_OR_DIE(m_targetingComp.has_value(), "ProjectileHitAbility::Update - m_targetingComp is null.");
    ASSERT_OR_DIE(m_aoeHitComp.has_value(), "ProjectileHitAbility::Update - m_aoeHitComp is null.");

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
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

	if (m_cooldownComp.has_value() && otherAoE.m_cooldownComp.has_value())
	{
		otherAoE.m_cooldownComp->m_accumulatedTime = m_cooldownComp->m_accumulatedTime;
	}
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
void AoEHitAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);
        
    if (m_cooldownComp.has_value())
    {
        m_cooldownComp->AppendDebugString(debugContext);
    }
    if (m_targetingComp.has_value())
    {
        m_targetingComp->AppendDebugString(debugContext);
    }
    if (m_critComp.has_value())
    {
        m_critComp->AppendDebugString(debugContext);
    }
    if (m_aoeHitComp.has_value())
    {
        m_aoeHitComp->AppendDebugString(debugContext);
    }
    if (m_aoeEffectComp.has_value())
    {
        m_aoeEffectComp->AppendDebugString(debugContext);
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
            payload.m_damage = rng.GetRandomFloatInRange(damageComp.GetMinDamage(), damageComp.GetMaxDamage());
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

        if (aoeHitComp.m_hasteOnHit.has_value())
        {
			AbilityHasteComponent const& hasteComp = aoeHitComp.m_hasteOnHit.value();
			payload.m_hasteDuration = hasteComp.m_duration;
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
void PassiveAoEAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

    if (m_aoeEffectComp.has_value())
    {
        m_aoeEffectComp->AppendDebugString(debugContext);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::ApplyModifier(TowerPayloadRunModifier const& modifier, CTags const& tags)
{
	for (auto& tag : modifier.GetDef().m_tagRequirements)
	{
		if (!tags.HasTag(tag))
		{
			return;
		}
	}

	if (m_aoeEffectComp.has_value())
	{
		m_aoeEffectComp->ApplyModifier(modifier);
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
void LaserAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation)
{
	ASSERT_OR_DIE(m_abilityDef, "LaserAbility::Update - m_abilityDef is null.");
	ASSERT_OR_DIE(m_targetingComp.has_value(), "LaserAbility::Update - m_targetingComp is null.");
	ASSERT_OR_DIE(m_onHitComp.has_value(), "LaserAbility::Update - m_onHitComp is null.");

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    if (deltaSeconds == 0.f)
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

    HitPayload const payload = RollDamageAndEffects(deltaSeconds);

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
void LaserAbility::CopyTransientDataTo(Ability&) const
{

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
void LaserAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

    if (m_targetingComp.has_value())
    {
        m_targetingComp->AppendDebugString(debugContext);
	}
    if (m_onHitComp.has_value())
    {
        m_onHitComp->AppendDebugString(debugContext);
    }
}



//----------------------------------------------------------------------------------------------------------------------
HitPayload LaserAbility::RollDamageAndEffects(float deltaSeconds) const
{
    HitPayload result;
    result.m_didCrit = false;
	result.m_damage = m_onHitComp.has_value() && m_onHitComp->m_damageOnHit.has_value() ? m_onHitComp->m_damageOnHit->GetMaxDamage() : 0.f;
    result.m_burn = m_onHitComp.has_value() && m_onHitComp->m_burnOnHit.has_value() ? m_onHitComp->m_burnOnHit->GetBurn() : 0.f;
    result.m_poison = m_onHitComp.has_value() && m_onHitComp->m_poisonOnHit.has_value() ? m_onHitComp->m_poisonOnHit->GetPoison() : 0.f;
    result.m_slowDuration = m_onHitComp.has_value() && m_onHitComp->m_slowOnHit.has_value() ? m_onHitComp->m_slowOnHit->GetDuration() : 0.f;
    result *= deltaSeconds;
	return result;
}
