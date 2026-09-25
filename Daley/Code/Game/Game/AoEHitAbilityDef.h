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
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

public:

	AbilityCooldownComponentDef		m_cooldownDef;
	AbilityTargetingComponentDef	m_targetingDef;
	AbilityCritComponentDef			m_critDef;
	AbilityAoEHitComponentDef		m_aoeHitDef;
	AbilityAoEEffectComponentDef	m_aoeEffectDef;
};