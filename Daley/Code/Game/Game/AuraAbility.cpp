// Bradley Christensen - 2022-2026
#include "AuraAbility.h"
#include "AuraAbilityDef.h"
#include "CAbility.h"
#include "CHealth.h"
#include "CTags.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCRenderer.h"
#include "DiscShaderCPU.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AuraAbility::AuraAbility(AuraAbilityDef const& def) : Ability(def)
{
	m_radius = def.m_targetingDef.m_maxRange;
	m_targetingComp = def.m_targetingDef;
	m_renderComp = def.m_renderDef;
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation)
{
    ASSERT_OR_DIE(m_abilityDef, "AuraAbility::Update - m_abilityDef is null.");

    // Write Dependencies
	auto& healthStorage = context.GetArrayStorage<CHealth>();
	auto& timeStorage = context.GetArrayStorage<CTime>();

	float deltaSeconds = context.m_deltaSeconds * timeDilation;

    m_targetingComp.UpdateCachedTiles(context, ability, location);
	if (!m_targetingComp.FindTargets(context, ability))
	{
		return;
	}

    for (EntityID entityID : m_targetingComp.m_targets)
    {

    }
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const
{
	CTags const& ownerTags = *context.GetComponentUnsafe<CTags>(m_owner);
	Rgba8 flavorTint = ownerTags.GetFlavorTint();

	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

    InstanceBuffer& discIBO = *renderer.GetInstanceBuffer(scRenderer.m_discInstanceBuffer);

    DiscRenderInstance discInstance;
    discInstance.m_position = Vec3(location, m_renderComp.m_depth);
    discInstance.m_radius = m_targetingComp.GetMaxRange(ability);
	discInstance.m_tint = flavorTint;
    discInstance.m_tint.a = 25;
    discIBO.AddInstance(discInstance);
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AuraAbility::DeepCopy() const
{
    AuraAbility* copy = new AuraAbility(*reinterpret_cast<AuraAbilityDef const*>(m_abilityDef));
    *copy = *this;
	return copy;
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::CopyTransientDataTo(Ability&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::AddDebugVerts(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability) const
{
    CTransform const* transform = context.GetComponentUnsafe<CTransform>(m_owner);
    if (transform)
    {
        AddDebugVertsAtLocation(out_vbo, context, ability, transform->m_pos);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::AddDebugVertsAtLocation(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability, Vec2 const& location) const
{
    float maxRange = m_targetingComp.GetMaxRange(ability);
    if (maxRange > 0.f)
    {
        VertexUtils::AddVertsForWireDisc2D(out_vbo, location, maxRange, 0.1f, 32, Rgba8::Orange);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void AuraAbility::AppendDebugString(EntityDebugContext& debugContext) const
{
    Ability::AppendDebugString(debugContext);

	m_targetingComp.AppendDebugString(debugContext);
    m_renderComp.AppendDebugString(debugContext);
}