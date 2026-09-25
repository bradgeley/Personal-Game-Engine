// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include "AbilityComponents.h"



struct LaserAbilityDef;
struct CPlaceable;
struct EntityDebugContext;
struct SystemContext;
struct Vec2;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
class LaserAbility : public Ability
{
public:

	LaserAbility() = default;
	explicit LaserAbility(LaserAbilityDef const& def);

	virtual void Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation) override;
	virtual void Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, CAbility const& ability, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	RolledOnHitComponent RollDamageAndEffects(CAbility const& ability, float deltaSeconds) const;

public:

	AbilityPrecisionTargetingComponent	m_targetingComp;
	AbilityCooldownComponent 			m_cooldownComp; // Cooldown is 0 for lasers, this is just a receptacle for attack speed modifiers
	AbilityCritComponent 				m_critComp;
	AbilityOnHitComponent				m_onHitComp;
	AbilityRenderComponent				m_renderComp;
	AbilityChainComponent				m_chainComp;
	AbilityMultishotComponent			m_multishotComp;
};