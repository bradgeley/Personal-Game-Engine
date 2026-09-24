// Bradley Christensen - 2022-2026
#include "Ability.h"
#include "AbilityComponentDefs.h"
#include "AbilityDef.h"
#include "AdjacentHitAbilityDef.h"
#include "AoEHitAbilityDef.h"
#include "CProjectile.h"
#include "CTime.h"
#include "DiscShaderCPU.h"
#include "EntityDef.h"
#include "LaserAbilityDef.h"
#include "PassiveAoEAbilityDef.h"
#include "ProjectileHitAbilityDef.h"
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
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AbilityTargetingComponent::AbilityTargetingComponent(AbilityTargetingComponentDef const& def)
{
    m_minRange = def.m_minRange;
	m_maxRange = def.m_maxRange;
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

        world.ForEachPathTileInRange(location, minRange, maxRange, [&](IntVec2 const& worldCoords)
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
    float minRange = GetMinRange();
	float maxRange = GetMaxRange();
    if (minRange > 0.f)
    {
        debugContext.m_debugString += StringUtils::StringF("Range: %.1f - %.1f\n", minRange, maxRange);
    }
    else
    {
        debugContext.m_debugString += StringUtils::StringF("Range: %.1f\n", maxRange);
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
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& transformStorage = context.GetArrayStorageConst<CTransform>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();

	float maxRange = GetMaxRange();

    m_targets.clear();

    static uint32_t aoeTargetingId = 0;
    aoeTargetingId++;

    for (IntVec2 const& cachedPathTile : m_cachedTilesInRange)
    {
        int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);

        CollisionBucket const& tileBucket = enemyLayer[tileIndex];

        for (EntityID entityID : tileBucket)
        {
            CHealth& healthComp = healthStorage[entityID];
			if (healthComp.m_lastProcessedBy == aoeTargetingId)
			{
                // This targeting component has already processed this entity, skip
				continue;
			}

			healthComp.m_lastProcessedBy = aoeTargetingId;

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
float AbilityCooldownComponent::GetCooldown() const
{
	float cooldown = m_cooldownSeconds;
    return cooldown;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponent::AppendDebugString(EntityDebugContext& debugContext) const
{
	float cooldown = GetCooldown();
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

	if (GetRadius() <= 0.f)
	{
		return false;
	}

	bool hasPayload = m_scaling.IsRelevant();
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

	m_scaling.AppendDebugString(debugContext);
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

    float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown();
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, StaticGameSettings::s_minTowerAttackCooldown);

    if (m_cooldownComp.m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

	// Write Dependencies
	auto& projectileStorage = context.GetMapStorage<CProjectile>();
    RandomNumberGenerator& rng = *context.GetSingleton<SCRandomNumberGenerator>().GetRNG();

    CRender& ownerRenderComp = *context.GetComponentUnsafe<CRender>(m_owner);
	Rgba8 const& ownerTint = ownerRenderComp.m_tint;

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
    spawnInfo.m_baseTint = ownerTint;

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
            ASSERT_OR_DIE(context.HasComponentUnsafe<CProjectile>(projectileID), "ProjectileHitAbility::Update - spawned projectile is missing CProjectile component.");
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
    // Todo: take into account tower attributes

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

    AbilityScalingComponent const& hitScaling = onHitComp.m_scaling;

    rolledHitPayload.m_physical = hitScaling.m_physical;
    rolledHitPayload.m_burn = hitScaling.m_burn;
    rolledHitPayload.m_poison = hitScaling.m_poison;
    rolledHitPayload.m_slowDuration = hitScaling.m_slow;
    rolledHitPayload.m_hasteDuration = hitScaling.m_haste;

    if (didCrit)
    {
        rolledHitPayload.m_physical *= critMultiplier;
        rolledHitPayload.m_burn *= critMultiplier;
        rolledHitPayload.m_poison *= critMultiplier;
    }

    if (onHitComp.m_aoeHitOnHit.IsRelevant())
    {
        RolledAoEHitComponent& rolledAoeHitResult = rolledHitResult.m_aoeHitOnHit;
        rolledAoeHitResult.m_radius = onHitComp.m_aoeHitOnHit.m_radius;
		HitPayload& rolledAoeHitPayload = rolledAoeHitResult.m_payload;
        rolledAoeHitPayload.m_didCrit = didCrit;

        AbilityScalingComponent const& aoeHitScaling = onHitComp.m_aoeHitOnHit.m_scaling;

        rolledAoeHitPayload.m_physical = aoeHitScaling.m_physical;
        rolledAoeHitPayload.m_burn = aoeHitScaling.m_burn;
        rolledAoeHitPayload.m_poison = aoeHitScaling.m_poison;
        rolledAoeHitPayload.m_slowDuration = aoeHitScaling.m_slow;
        rolledAoeHitPayload.m_hasteDuration = aoeHitScaling.m_haste;

        if (didCrit)
        {
            rolledAoeHitPayload.m_physical *= critMultiplier;
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

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown();
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, StaticGameSettings::s_minTowerAttackCooldown);

    if (m_aoeHitComp.m_renderComp.has_value())
    {
		m_aoeHitComp.m_renderComp->m_renderDurationRemaining -= context.m_deltaSeconds;
    }

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

        if (m_aoeHitComp.m_renderComp.has_value())
        {
            m_aoeHitComp.m_renderComp->m_renderDurationRemaining = m_aoeHitComp.m_renderComp->m_renderDuration;
        }

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
void AoEHitAbility::Render(SystemContext const& context, Vec2 const& location) const
{
    if (!m_aoeHitComp.m_renderComp.has_value() || m_aoeHitComp.m_renderComp->m_renderDurationRemaining <= 0.f)
	{
		return;
	}

    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

	float alphaT = m_aoeHitComp.m_renderComp->m_renderDurationRemaining / m_aoeHitComp.m_renderComp->m_renderDuration;
    float alpha;
    if (alphaT < 0.5f)
    {
        // Ramp up alpha to max
		alpha = MathUtils::RangeMapClamped(alphaT, 0.f, 0.5f, 0.f, m_aoeHitComp.m_renderComp->m_tint.a);
    }
    else
    {
        // Ramp down to 0
        alpha = MathUtils::RangeMapClamped(alphaT, 0.5f, 1.f, m_aoeHitComp.m_renderComp->m_tint.a, 0.f);
    }

    DiscRenderInstance discInstance;
    discInstance.m_position = Vec3(location, m_aoeHitComp.m_renderComp->m_depth);
	discInstance.m_radius = (1.f - alphaT) * m_targetingComp.GetMaxRange(); // Make it look like an explosion expanding outwards
    discInstance.m_tint = m_aoeHitComp.m_renderComp->m_tint;
    discInstance.m_tint.a = static_cast<uint8_t>(alpha);

    discIBO.AddInstance(discInstance);
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
		AbilityScalingComponent const& hitScaling = aoeHitComp.m_scaling;

		payload.m_physical = hitScaling.m_physical;
		payload.m_burn = hitScaling.m_burn;
		payload.m_poison = hitScaling.m_poison;
		payload.m_slowDuration = hitScaling.m_slow;
		payload.m_hasteDuration = hitScaling.m_haste;

		if (didCrit)
		{
			payload.m_physical *= critMultiplier;
			payload.m_burn *= critMultiplier;
			payload.m_poison *= critMultiplier;
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
void PassiveAoEAbility::Shutdown(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::Update(SystemContext const& context, Vec2 const& location, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "PassiveAoEAbility::Update - m_abilityDef is null.");

    // Write Dependencies
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();

    HitPayload const& payload = GetDotPayload(context.m_deltaSeconds * timeDilation);
    if (!payload.HasValue())
    {
        return;
    }

    m_targetingComp.UpdateCachedTiles(context, location);
	if (!m_targetingComp.FindTargets(context))
	{
		return;
	}

    for (EntityID entityID : m_targetingComp.m_targets)
    {
        if (payload.IsRelevantToHealth())
        {
            CHealth& healthComp = healthStorage[entityID];
            healthComp.TakePayload(payload);
        }

        if (payload.IsRelevantToTime())
        {
            CTime& timeComp = timeStorage[entityID];
            timeComp.m_remainingSlowDuration += payload.m_slowDuration;
            timeComp.m_remainingHasteDuration += payload.m_hasteDuration;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::Render(SystemContext const& context, Vec2 const& location) const
{
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

    DiscRenderInstance discInstance;
    discInstance.m_position = Vec3(location, m_aoeEffectComp.m_renderComp.m_depth);
    discInstance.m_radius = m_targetingComp.GetMaxRange();
	discInstance.m_tint = m_aoeEffectComp.m_renderComp.m_tint;
    discIBO.AddInstance(discInstance);
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
HitPayload PassiveAoEAbility::GetDotPayload(float deltaSeconds) const
{
	HitPayload payload;

	payload.m_physical = m_aoeEffectComp.m_scaling.m_physical * deltaSeconds;
	payload.m_burn = m_aoeEffectComp.m_scaling.m_burn * deltaSeconds;
	payload.m_poison = m_aoeEffectComp.m_scaling.m_poison * deltaSeconds;
	payload.m_slowDuration = m_aoeEffectComp.m_scaling.m_slow * deltaSeconds;
	payload.m_hasteDuration = m_aoeEffectComp.m_scaling.m_haste * deltaSeconds;
    
    return payload;
}



//----------------------------------------------------------------------------------------------------------------------
AdjacentHitAbility::AdjacentHitAbility(AdjacentHitAbilityDef const& def) : Ability(def)
{
	m_cooldownComp = def.m_cooldownDef;
	m_scaling = def.m_scaling;
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbility::Update(SystemContext const& context, Vec2 const&, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "AdjacentHitAbility::Update - m_abilityDef is null.");

    float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown();
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, StaticGameSettings::s_minTowerAttackCooldown);

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

            // Todo: buff adjacent towers with stats

            if (m_scaling.m_haste > 0.f || m_scaling.m_slow > 0.f)
            {
                CTime& time = timeStorage[targetID];
                time.m_remainingHasteDuration += m_scaling.m_haste;
                time.m_remainingSlowDuration += m_scaling.m_slow;
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
	m_scaling.AppendDebugString(debugContext);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityRenderComponent::AbilityRenderComponent(AbilityRenderComponentDef const& def)
{
    m_tint = def.m_tint;
	m_depth = def.m_depth;
	m_renderDuration = def.m_renderDuration;
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

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    if (deltaSeconds == 0.f)
    {
        return;
    }

    BitMask healthBit = context.GetComponentBitMask<CHealth>();
    BitMask timeBit = context.GetComponentBitMask<CTime>();

	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();
	auto& transformStorage = context.GetArrayStorage<CTransform>();
	auto& collisionEffectStorage = context.GetArrayStorage<CCollisionEffect>();
     
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
                CHealth& healthComp = healthStorage[target];
                healthComp.TakePayload(chainPayload);
            }

            if (chainPayload.IsRelevantToTime() && context.HasComponentsUnsafe(target.GetIndex(), timeBit))
            {
                CTime& timeComp = timeStorage[target];
                timeComp.m_remainingSlowDuration += chainPayload.m_slowDuration;
            }

            if (m_onHitComp.m_aoeEffectOnHit.IsRelevant())
            {
                CTransform const& targetTransform = transformStorage[target];

                SpawnInfo aoeEffectSpawnInfo;
                aoeEffectSpawnInfo.m_spawnPos = targetTransform.m_pos;
                aoeEffectSpawnInfo.m_spawnLifetime = m_onHitComp.m_aoeEffectOnHit.GetDuration();
                aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(m_onHitComp.m_aoeEffectOnHit.m_aoeEffectDefName);
                aoeEffectSpawnInfo.m_spawnScale = m_onHitComp.m_aoeEffectOnHit.GetRadius();

                EntityID aoeEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);
                if (context.IsValid(aoeEffect))
                {
                    CCollisionEffect& aoeEffectComp = collisionEffectStorage[aoeEffect];
                    aoeEffectComp.InitializeFromAoEEffect(m_onHitComp.m_aoeEffectOnHit);
                }
            }
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbility::Render(SystemContext const& context, Vec2 const& location) const
{
    // TODO: Add to a vbo and render all lasers at once, to improve laser spam perf

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

            if (CTransform const* transform = context.GetComponentUnsafe<CTransform>(target))
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

	m_cooldownComp.AppendDebugString(debugContext);
	m_critComp.AppendDebugString(debugContext);
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
	result.m_physical = m_onHitComp.m_scaling.m_physical;
    result.m_burn = m_onHitComp.m_scaling.m_burn;
    result.m_poison = m_onHitComp.m_scaling.m_poison;
    result.m_slowDuration = m_onHitComp.m_scaling.m_slow;
	result.m_hasteDuration = m_onHitComp.m_scaling.m_haste;
    result *= deltaSeconds;

    float critMulti = StaticGameSettings::s_baseCritMultiplier + m_critComp.m_critMulti;
	float avgDPSMultiplierFromCrit = 1.f + m_critComp.m_critChance * (critMulti - 1.f);

    result *= avgDPSMultiplierFromCrit;

	return result;
}
