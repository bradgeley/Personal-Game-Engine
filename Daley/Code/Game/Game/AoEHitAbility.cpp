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
	m_splashComp = def.m_splashDef;
	m_puddleComp = def.m_puddleDef;
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation /*= 1.f*/)
{
    ASSERT_OR_DIE(m_abilityDef, "ProjectileHitAbility::Update - m_abilityDef is null.");

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown(ability);
    timeBetweenAttacks = MathUtils::Max(timeBetweenAttacks, StaticGameSettings::s_minTowerAttackCooldown);

    if (m_splashComp.m_renderComp.has_value())
    {
		m_splashComp.m_renderComp->m_renderDurationRemaining -= context.m_deltaSeconds;
    }

    if (m_cooldownComp.m_accumulatedTime <= timeBetweenAttacks)
    {
        return;
    }

	// Write Dependencies
    auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();
	auto& puddleStorage = context.GetArrayStorage<CPuddle>();
    RandomNumberGenerator& rng = *context.GetSingleton<SCRandomNumberGenerator>().GetRNG();
	// CAbility (bc this is an ability in a CAbility that can update itself)
	// Spawn Entities (All)

	BitMask puddleBit = context.GetComponentBitMask<CPuddle>();

	CTags const& ownerTags = *context.GetComponentUnsafe<CTags>(m_owner);
    Rgba8 flavorTint = ownerTags.GetFlavorTint();
    //auto const& ownerFlavors = ownerTags.GetFlavorDefs();
    //int numOwnerFlavors = ownerTags.GetNumFlavorTags();

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

        RolledOnHitComponent rolledHit = RollDamageAndEffects(ability, rng);
        if (!rolledHit.IsRelevant())
        {
            continue;
        }

        if (m_splashComp.m_renderComp.has_value())
        {
            m_splashComp.m_renderComp->m_renderDurationRemaining = m_splashComp.m_renderComp->m_renderDuration;
        }

        if (rolledHit.m_splashOnHit.IsRelevant())
        {
			HitPayload const& aoePayload = rolledHit.m_splashOnHit.m_payload;
            for (EntityID entityID : m_targetingComp.m_targets)
            {
                if (aoePayload.IsRelevantToHealth())
                {
                    CHealth& healthComp = healthStorage[entityID];
                    healthComp.TakePayload(aoePayload);
                }

                if (aoePayload.IsRelevantToTime())
                {
                    CTime& timeComp = timeStorage[entityID];
                    timeComp.TakePayload(aoePayload);
                }
            }
        }

        if (rolledHit.m_puddleOnHit.IsRelevant())
        {
            SpawnInfo puddleSpawnInfo;
            puddleSpawnInfo.m_spawnPos = location;
            puddleSpawnInfo.m_spawnLifetime = rolledHit.m_puddleOnHit.m_durationSeconds;
            puddleSpawnInfo.m_def = EntityDef::GetEntityDef(rolledHit.m_puddleOnHit.m_puddleDefName);
            puddleSpawnInfo.m_spawnScale = maxRange; // Initial radius is assumed to be 1

            EntityID puddle = SEntityFactory::SpawnEntity(context, puddleSpawnInfo);
			if (!context.IsValid(puddle))
			{
                continue; // at entity limit?
			}

			ASSERT_OR_DIE(context.HasComponents(puddle, puddleBit), "AoEHitAbility::Update - Spawned entity does not have CPuddle component.");

            // Pass along damage, color, to aoe effect
            CPuddle& puddleComp = puddleStorage[puddle];

            rolledHit.m_puddleOnHit.m_renderComp.m_tint = flavorTint;
            rolledHit.m_puddleOnHit.m_renderComp.m_tint.a = 25;

            puddleComp.Initialize(rolledHit.m_puddleOnHit);
		}
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const
{
    if (!m_splashComp.m_renderComp.has_value() || m_splashComp.m_renderComp->m_renderDurationRemaining <= 0.f)
	{
		return;
	}

	CTags const& ownerTags = *context.GetComponentUnsafe<CTags>(m_owner);
	Rgba8 flavorTint = ownerTags.GetFlavorTint();

    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

    constexpr float maxAlpha = 50;

	float alphaT = m_splashComp.m_renderComp->m_renderDurationRemaining / m_splashComp.m_renderComp->m_renderDuration;
    float alpha;
    if (alphaT < 0.5f)
    {
        // Ramp up alpha to max
		alpha = MathUtils::RangeMapClamped(alphaT, 0.f, 0.5f, 0.f, maxAlpha);
    }
    else
    {
        // Ramp down to 0
        alpha = MathUtils::RangeMapClamped(alphaT, 0.5f, 1.f, maxAlpha, 0.f);
    }

    DiscRenderInstance discInstance;
    discInstance.m_position = Vec3(location, m_splashComp.m_renderComp->m_depth);
	discInstance.m_radius = (1.f - alphaT) * m_targetingComp.GetMaxRange(ability); // Make it look like an explosion expanding outwards
    discInstance.m_tint = flavorTint;
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
void AoEHitAbility::AddDebugVerts(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability) const
{
	CTransform const* transform = context.GetComponentUnsafe<CTransform>(m_owner);
	if (transform)
	{
		AddDebugVertsAtLocation(out_vbo, context, ability, transform->m_pos);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbility::AddDebugVertsAtLocation(VertexBuffer& out_vbo, SystemContext const&, CAbility const& ability, Vec2 const& location) const
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
	m_splashComp.AppendDebugString(debugContext);
	m_puddleComp.AppendDebugString(debugContext);
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

    if (ability.m_abilityFlags.HasFlag(AbilityFlag::Splash) && m_splashComp.IsRelevant())
    {
        result.m_splashOnHit = m_splashComp.CalculateSplashPayload(ability, didCrit, critMultiplier);
    }

    if (ability.m_abilityFlags.HasFlag(AbilityFlag::Puddle) && m_puddleComp.IsRelevant())
    {
        result.m_puddleOnHit = m_puddleComp.CalculatePuddlePayload(ability);
    }

    return result;
}