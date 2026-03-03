// Bradley Christensen - 2022-2026
#include "Ability.h"
#include "CProjectile.h"
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



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitAbility::ProjectileHitAbility(ProjectileHitAbilityDef const& def)
{
    m_abilityDef = &def;
    m_projectileDefName = def.m_projectileDefName;
    m_projSpeedUnitsPerSec = def.m_projSpeed;

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
	ASSERT_OR_DIE(m_critComp.has_value(), "ProjectileHitAbility::Update - m_critComp is null.");
	ASSERT_OR_DIE(m_onHitComp.has_value(), "ProjectileHitAbility::Update - m_onHitComp is null.");

	m_cooldownComp->m_accumulatedTime += deltaSeconds;

	constexpr float maxAttacksPerSecond = 1000.f;

	float timeBetweenAttacks = m_cooldownComp->m_cooldownSeconds;
    ASSERT_OR_DIE(timeBetweenAttacks > 0.f, "ProjectileHitAbility::Update - time between attacks is 0 or negative.");

    // Find targets
    SCCollision const& collision = g_ecs->GetSingleton<SCCollision>();
    SCFlowField const& flowfield = g_ecs->GetSingleton<SCFlowField>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
    auto const& healthStorage = g_ecs->GetArrayStorage<CHealth>();

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

	EntityID targetID = EntityID::Invalid;
    if (m_targetingComp->m_targetingMode == AbilityTargetingMode::ClosestToGoal)
    {
        static std::vector<EntityID> closestToGoalValidTargets;
        closestToGoalValidTargets.clear();

		float closestToGoalDist = FLT_MAX;
		int closestToGoalTileIndex = -1;
        for (IntVec2 const& cachedPathTile : m_targetingComp->m_cachedPathTilesInRange)
        {
            int tileIndex = world.m_tiles.GetIndexForCoords(cachedPathTile);
            float distance = flowfield.m_toGoalFlowField.m_distanceField.Get(tileIndex);
            if (distance < closestToGoalDist && !collision.m_tileBuckets[tileIndex].empty()) // todo: only check "Enemy" collision layer
            {
				bool tileHasValidTarget = false;

                for (auto& entityID : collision.m_tileBuckets[tileIndex])
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
            for (auto& entityID : collision.m_tileBuckets[closestToGoalTileIndex])
            {
                CHealth const* healthComp = healthStorage.Get(entityID.GetIndex());
                if (healthComp && healthComp->GetIsTargetable() && !healthComp->GetHealthReachedZero())
                {
					closestToGoalValidTargets.push_back(entityID);
                }
            }

			ASSERT_OR_DIE(!closestToGoalValidTargets.empty(), "ProjectileHitAbility::Update - closest to goal tile has no valid targets. Should be impossible.");

            int randomIndex = g_rng->GetRandomIntInRange(0, static_cast<int>(closestToGoalValidTargets.size()) - 1);
            targetID = closestToGoalValidTargets[randomIndex];
        }
    }

    if (targetID != EntityID::Invalid)
    {
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

            // Copy ability data to proj, snapshotted
			CProjectile& projComp = *g_ecs->GetComponent<CProjectile>(projectileID);
			projComp.m_targetID = targetID;
			projComp.m_targetPos = std::nullopt;
            projComp.m_accumulatedTime += m_cooldownComp->m_accumulatedTime;
			projComp.m_projSpeedUnitsPerSec = m_projSpeedUnitsPerSec;
            projComp.m_critComp = m_critComp;
			projComp.m_onHitComp = m_onHitComp;
        }
    }
    else
    {
        m_cooldownComp->m_accumulatedTime = MathUtils::Clamp(m_cooldownComp->m_accumulatedTime, 0.f, timeBetweenAttacks);
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
void ProjectileHitAbility::GetDebugString(std::string& out_string) const
{
    out_string += StringUtils::StringF("Ability Type: Projectile Hit\n");
    out_string += StringUtils::StringF("Projectile Def: %s\n", m_projectileDefName.ToString().c_str());
    out_string += StringUtils::StringF("Damage: %.1f\n", m_onHitComp->m_damageOnHit->m_minDamage);
    out_string += StringUtils::StringF("Attack Cooldown: %.1f\n", m_cooldownComp->m_cooldownSeconds);
    out_string += StringUtils::StringF("Projectile Speed: %.1f\n", m_projSpeedUnitsPerSec);
    out_string += StringUtils::StringF("Min Range: %.1f\n", m_targetingComp->m_minRange);
	out_string += StringUtils::StringF("Max Range: %.1f\n", m_targetingComp->m_maxRange);
}



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
AbilityCooldownComponent::AbilityCooldownComponent(AbilityCooldownComponentDef const& def)
{
	m_cooldownSeconds = def.m_cooldownSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCritComponent::AbilityCritComponent(AbilityCritComponentDef const& def)
{
    m_critChance = def.m_critChance;
	m_critMulti = def.m_critMulti;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDamageComponent::AbilityDamageComponent(AbilityDamageComponentDef const& def)
{
    m_minDamage = def.m_minDamage;
	m_maxDamage = def.m_maxDamage;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityBurnComponent::AbilityBurnComponent(AbilityBurnComponentDef const& def)
{
	m_burn = def.m_burn;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityPoisonComponent::AbilityPoisonComponent(AbilityPoisonComponentDef const& def)
{
	m_poison = def.m_poison;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityOnHitComponent::AbilityOnHitComponent(AbilityOnHitComponentDef const& def)
{
    m_damageOnHit = def.m_damageOnHit;
	m_poisonOnHit = def.m_poisonOnHit;
	m_burnOnHit = def.m_burnOnHit;
	m_aoeHitOnHit = def.m_aoeHitOnHit;
	m_aoeEffectOnHit = def.m_aoeEffectOnHit;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoeHitComponent::AbilityAoeHitComponent(AbilityAoeHitComponentDef const& def)
{
    m_radius = def.m_radius;
    m_damageOnHit = def.m_damageOnHit;
    m_poisonOnHit = def.m_poisonOnHit;
	m_burnOnHit = def.m_burnOnHit;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoeEffectComponent::AbilityAoeEffectComponent(AbilityAoeEffectComponentDef const& def)
{
    m_radius = def.m_radius;
    m_durationSeconds = def.m_durationSeconds;
    m_damagePerSecond = def.m_damagePerSecond;
    m_poisonPerSecond = def.m_poisonPerSecond;
	m_burnPerSecond = def.m_burnPerSecond;
}
