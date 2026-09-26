// Bradley Christensen - 2022-2026
#include "PassiveAoEAbility.h"
#include "CHealth.h"
#include "CTime.h"
#include "SCRenderer.h"
#include "DiscShaderCPU.h"
#include "PassiveAoEAbilityDef.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
PassiveAoEAbility::PassiveAoEAbility(PassiveAoEAbilityDef const& def) : Ability(def)
{
	m_targetingComp = def.m_targetingDef;
	m_aoeEffectComp = def.m_aoeEffectDef;
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "PassiveAoEAbility::Update - m_abilityDef is null.");

    // Write Dependencies
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();

	float deltaSeconds = context.m_deltaSeconds * timeDilation;
	RolledAoEEffectComponent payload = m_aoeEffectComp.CalculateDotPayload(ability, deltaSeconds);
    if (!payload.IsRelevant())
    {
        return;
    }

    m_targetingComp.UpdateCachedTiles(context, ability, location);
	if (!m_targetingComp.FindTargets(context, ability))
	{
		return;
	}

    for (EntityID entityID : m_targetingComp.m_targets)
    {
        if (payload.m_payload.IsRelevantToHealth())
        {
            CHealth& healthComp = healthStorage[entityID];
            healthComp.TakePayload(payload.m_payload);
        }

        if (payload.m_payload.IsRelevantToTime())
        {
            CTime& timeComp = timeStorage[entityID];
			timeComp.TakePayload(payload.m_payload);
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbility::Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const
{
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

    DiscRenderInstance discInstance;
    discInstance.m_position = Vec3(location, m_aoeEffectComp.m_renderComp.m_depth);
    discInstance.m_radius = m_targetingComp.GetMaxRange(ability);
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
void PassiveAoEAbility::AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const&, CAbility const& ability,    Vec2 const& location) const
{
    float maxRange = m_targetingComp.GetMaxRange(ability);
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