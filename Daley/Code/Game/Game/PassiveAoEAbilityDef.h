// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityComponentDefs.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
struct PassiveAoEAbilityDef : public AbilityDef
{
public:

	explicit PassiveAoEAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

public:

	AbilityTargetingComponentDef	m_targetingDef;
	AbilityAoEEffectComponentDef	m_aoeEffectDef;
};