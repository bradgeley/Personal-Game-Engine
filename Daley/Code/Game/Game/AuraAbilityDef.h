// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityComponentDefs.h"
#include "AbilityDef.h"



//----------------------------------------------------------------------------------------------------------------------
enum class AuraType : uint8_t
{
	Invalid,
	Protection,
	Haste,
	Slow,
	BurnSuppression,
	PoisonSuppression,
	Healing,
};



//----------------------------------------------------------------------------------------------------------------------
struct AuraAbilityDef : public AbilityDef
{
public:

	explicit AuraAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;

public:

	AuraType m_auraType = AuraType::Invalid;
	AbilityTargetingComponentDef	m_targetingDef;
	AbilityRenderComponentDef		m_renderDef;
};