// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityComponentDefs.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
struct AdjacentHitAbilityDef : public AbilityDef
{
public:

	explicit AdjacentHitAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;

public:

	AbilityCooldownComponentDef	m_cooldownDef;
	AbilityScalingComponentDef	m_scaling;
	// todo: physical/burn/poison buffs on hit
};