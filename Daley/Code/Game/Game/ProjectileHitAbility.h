// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include "AbilityComponents.h"
#include "Engine/Core/Name.h"



struct ProjectileHitAbilityDef;
class RandomNumberGenerator;



//----------------------------------------------------------------------------------------------------------------------
class ProjectileHitAbility : public Ability
{
public:

	ProjectileHitAbility() = default;
	explicit ProjectileHitAbility(ProjectileHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, CAbility const& ability, Vec2 const& location, float timeDilation) override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, CAbility const& ability, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	RolledOnHitComponent RollDamageAndEffects(CAbility const& ability, RandomNumberGenerator& rng) const;

public:

	Name m_projectileDefName = Name::Invalid;
	float m_projSpeed = 1.f;

	AbilityCooldownComponent m_cooldownComp;
	AbilityPrecisionTargetingComponent m_targetingComp;
	AbilityCritComponent m_critComp;
	AbilityChainComponent m_chainComp;
	AbilityMultishotComponent m_multishotComp;
	AbilityOnHitComponent m_onHitComp;
};