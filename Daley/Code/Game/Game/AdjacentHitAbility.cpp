// Bradley Christensen - 2022-2026
#include "AdjacentHitAbility.h"
#include "AdjacentHitAbilityDef.h"
#include "CPlaceable.h"
#include "CTime.h"
#include "WorldSettings.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AdjacentHitAbility::AdjacentHitAbility(AdjacentHitAbilityDef const& def) : Ability(def)
{
	m_cooldownComp = def.m_cooldownDef;
	m_scaling = def.m_scaling;
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbility::Update(SystemContext const& context, CAbility const& ability, Vec2 const&, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "AdjacentHitAbility::Update - m_abilityDef is null.");

    float deltaSeconds = context.m_deltaSeconds * timeDilation;
    m_cooldownComp.m_accumulatedTime += deltaSeconds;

    float timeBetweenAttacks = m_cooldownComp.GetCooldown(ability);
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
void AdjacentHitAbility::AddDebugVerts(VertexBuffer& vbo, CPlaceable const& placeable, CAbility const&,  Vec2 const&) const
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