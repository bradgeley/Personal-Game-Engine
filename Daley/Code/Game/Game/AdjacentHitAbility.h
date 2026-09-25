// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include "AbilityComponents.h"



struct AdjacentHitAbilityDef; 
struct CPlaceable;
struct EntityDebugContext;
struct SystemContext;
struct Vec2;
class VertexBuffer;




//----------------------------------------------------------------------------------------------------------------------
class AdjacentHitAbility : public Ability
{
public:

	AdjacentHitAbility() = default;
	explicit AdjacentHitAbility(AdjacentHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation) override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, CAbility const& ability, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

public:

	AbilityCooldownComponent			m_cooldownComp;
	AbilityAdjacentTargetingComponent	m_targetingComp;
	AbilityScalingComponent				m_scaling;
};