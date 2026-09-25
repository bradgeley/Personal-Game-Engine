// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include "AbilityComponents.h"
#include "HitPayload.h"



struct AoEHitAbilityDef;
struct CPlaceable;
struct EntityDebugContext;
struct SystemContext;
struct Vec2;
class RandomNumberGenerator;



//----------------------------------------------------------------------------------------------------------------------
class AoEHitAbility : public Ability
{
public:

	AoEHitAbility() = default;
	explicit AoEHitAbility(AoEHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation) override;
	virtual void Render(SystemContext const& context, CAbility const& ability, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, CAbility const& ability, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	RolledOnHitComponent RollDamageAndEffects(CAbility const& ability, RandomNumberGenerator& rng) const;

public:

	AbilityCooldownComponent		m_cooldownComp;
	AbilityAoETargetingComponent	m_targetingComp;
	AbilityCritComponent			m_critComp;
	AbilityAoEHitComponent			m_aoeHitComp;
	AbilityAoEEffectComponent		m_aoeEffectComp;
};