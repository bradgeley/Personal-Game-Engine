// Bradley Christensen - 2022-2026
#include "ProjectileHitAbility.h"
#include "CProjectile.h"
#include "EntityDef.h"
#include "ProjectileHitAbilityDef.h"
#include "SEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



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
void ProjectileHitAbility::Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");

    float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown(ability);
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
    m_targetingComp.UpdateCachedTiles(context, ability, location);

	int maxTargets = m_multishotComp.GetAdditionalTargets(ability) + 1;
	int maxChains = m_chainComp.GetMaxChains(ability);
	float chainDistance = m_chainComp.GetChainDistance(ability);

	// todo: chain chance and payload multi

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
            projComp.m_onHitComp = RollDamageAndEffects(ability, rng);
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
void ProjectileHitAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, CAbility const& ability, Vec2 const& location) const
{
    float maxRange = m_targetingComp.GetMaxRange(ability);

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
RolledOnHitComponent ProjectileHitAbility::RollDamageAndEffects(CAbility const& ability, RandomNumberGenerator& rng) const
{
	RolledOnHitComponent rolledHitResult;

    bool didCrit = false;
	float critChance = m_critComp.GetCritChance(ability);
    float critMultiplier = m_critComp.GetCritMultiplier(ability);
	if (critChance > 0.f)
	{
        float critRoll = rng.GetRandomFloatZeroToOne();
        didCrit = critRoll < critChance;
	}

	if (ability.m_abilityFlags.HasFlag(AbilityFlag::Hit) && m_onHitComp.m_scaling.IsRelevant())
	{
        rolledHitResult.m_payload = m_onHitComp.m_scaling.CalculateHitPayload(ability, didCrit, critMultiplier);
	}  

    if (ability.m_abilityFlags.HasFlag(AbilityFlag::Splash) && m_onHitComp.m_aoeHitOnHit.IsRelevant())
    {
        rolledHitResult.m_aoeHitOnHit = m_onHitComp.m_aoeHitOnHit.CalculateSplashPayload(ability, didCrit, critMultiplier);
    }

    if (ability.m_abilityFlags.HasFlag(AbilityFlag::Puddle) && m_onHitComp.m_aoeEffectOnHit.IsRelevant())
    {
		rolledHitResult.m_aoeEffectOnHit = m_onHitComp.m_aoeEffectOnHit.CalculatePuddlePayload(ability);
    }

	return rolledHitResult;
}