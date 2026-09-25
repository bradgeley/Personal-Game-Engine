// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityComponentDefs.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
struct LaserAbilityDef : public AbilityDef
{
public:

	explicit LaserAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

public:

	AbilityTargetingComponentDef	m_targetingDef;
	AbilityOnHitComponentDef		m_onHitDef;
	AbilityRenderComponentDef		m_renderDef;
	AbilityChainComponentDef		m_chainDef;
	AbilityMultishotComponentDef	m_multishotDef;
};