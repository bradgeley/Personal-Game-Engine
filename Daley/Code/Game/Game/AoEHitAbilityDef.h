// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityComponentDefs.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
struct AoEHitAbilityDef : public AbilityDef
{
public:

	explicit AoEHitAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;

public:

	AbilityCooldownComponentDef		m_cooldownDef;
	AbilityTargetingComponentDef	m_targetingDef;
	AbilityCritComponentDef			m_critDef;
	AbilitySplashComponentDef		m_splashDef;
	AbilityPuddleComponentDef		m_puddleDef;
};