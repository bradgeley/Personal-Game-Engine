// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include "AbilityComponents.h"



struct AuraAbilityDef;
struct EntityDebugContext;
struct SystemContext;
struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
class AuraAbility : public Ability
{
public:

	AuraAbility() = default;
	explicit AuraAbility(AuraAbilityDef const& def);

	virtual void Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation) override;
	virtual void Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability) const override;
	virtual void AddDebugVertsAtLocation(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	AuraAbilityDef const* GetDef() const;

public:

	float m_radius = 0.f;
	AbilityAoETargetingComponent	m_targetingComp;
	AbilityRenderComponent			m_renderComp;
};