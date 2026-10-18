// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include "AbilityComponents.h"



struct PassiveAoEAbilityDef;
struct CAbility;;
struct CPlaceable;
struct EntityDebugContext;
struct SystemContext;
struct Vec2;



//----------------------------------------------------------------------------------------------------------------------
class PassiveAoEAbility : public Ability
{
public:

	PassiveAoEAbility() = default;
	explicit PassiveAoEAbility(PassiveAoEAbilityDef const& def);

	virtual void Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation) override;
	virtual void Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability) const override;
	virtual void AddDebugVertsAtLocation(VertexBuffer& out_vbo, SystemContext const& context, CAbility const& ability, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	virtual HitPayload GetDotPayload(float deltaSeconds) const;

public:

	AbilityAoETargetingComponent	m_targetingComp;
	AbilityPuddleComponent			m_puddleComp;
};