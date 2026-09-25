// Bradley Christensen - 2022-2026
#include "AoEHitAbility.h"
#include "AoEHitAbilityDef.h"
#include "CTime.h"
#include "DiscShaderCPU.h"
#include "EntityDef.h"
#include "SEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



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
void AoEHitAbility::Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation /*= 1.f*/)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown(ability);
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
	m_targetingComp.UpdateCachedTiles(context, ability, location);

	float maxRange = m_targetingComp.GetMaxRange(ability);

	if (!m_targetingComp.FindTargets(context, ability))
    {
        // No targets in range, clamp cooldown
        m_cooldownComp.m_accumulatedTime = MathUtils::Clamp(m_cooldownComp.m_accumulatedTime, 0.f, timeBetweenAttacks);
        return;
    }

    while (m_cooldownComp.m_accumulatedTime > timeBetweenAttacks)
    {
        m_cooldownComp.m_accumulatedTime -= timeBetweenAttacks;

        RolledOnHitComponent const& result = RollDamageAndEffects(ability, rng);
        if (!result.IsRelevant())
        {
            continue;
        }

        if (m_aoeHitComp.m_renderComp.has_value())
        {
            m_aoeHitComp.m_renderComp->m_renderDurationRemaining = m_aoeHitComp.m_renderComp->m_renderDuration;
        }

        if (result.m_aoeHitOnHit.IsRelevant())
        {
			HitPayload const& aoePayload = result.m_aoeHitOnHit.m_payload;
            for (EntityID entityID : m_targetingComp.m_targets)
            {
                if (aoePayload.IsRelevantToHealth() && context.HasComponents(entityID, healthBit))
                {
                    CHealth& healthComp = healthStorage[entityID];
                    healthComp.TakePayload(aoePayload);
                }

                if (aoePayload.IsRelevantToTime() && context.HasComponents(entityID, timeBit))
                {
                    CTime& timeComp = timeStorage[entityID];
                    timeComp.TakePayload(aoePayload);
                }
            }
        }

        if (result.m_aoeEffectOnHit.IsRelevant())
        {
            SpawnInfo aoeEffectSpawnInfo;
            aoeEffectSpawnInfo.m_spawnPos = location;
            aoeEffectSpawnInfo.m_spawnLifetime = result.m_aoeEffectOnHit.m_durationSeconds;
            aoeEffectSpawnInfo.m_def = EntityDef::GetEntityDef(result.m_aoeEffectOnHit.m_aoeEffectDefName);
            aoeEffectSpawnInfo.m_spawnScale = maxRange; // Initial radius is assumed to be 1

            EntityID aoeEffect = SEntityFactory::SpawnEntity(context, aoeEffectSpawnInfo);

            // Pass along damage, color, to aoe effect
            if (context.HasComponents(aoeEffect, collisionEffectBit))
            {
                CCollisionEffect& aoeEffectComp = collisionEffectStorage[aoeEffect];
                aoeEffectComp.Initialize(result.m_aoeEffectOnHit);
            }
		}
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const
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
	discInstance.m_radius = (1.f - alphaT) * m_targetingComp.GetMaxRange(ability); // Make it look like an explosion expanding outwards
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
void AoEHitAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, CAbility const& ability, Vec2 const& location) const
{
    float maxRange = m_targetingComp.GetMaxRange(ability);
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
RolledOnHitComponent AoEHitAbility::RollDamageAndEffects(CAbility const& ability, RandomNumberGenerator& rng) const
{
	RolledOnHitComponent result;

    bool didCrit = false;
    float critChance = m_critComp.GetCritChance(ability);
    float critMultiplier = m_critComp.GetCritMultiplier(ability);
    if (critChance > 0.f)
    {
        float critRoll = rng.GetRandomFloatZeroToOne();
        didCrit = critRoll < critChance;
    }

    if (ability.m_abilityFlags.HasFlag(AbilityFlag::Splash) && m_aoeHitComp.IsRelevant())
    {
        result.m_aoeHitOnHit = m_aoeHitComp.CalculateSplashPayload(ability, didCrit, critMultiplier);
    }

    if (ability.m_abilityFlags.HasFlag(AbilityFlag::Puddle) && m_aoeEffectComp.IsRelevant())
    {
        result.m_aoeEffectOnHit = m_aoeEffectComp.CalculatePuddlePayload(ability);
    }

    return result;
}