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
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

	virtual void SwirlInto(AbilityDef& other) const override;
	virtual bool ReceiveSwirl(ProjectileHitAbilityDef const& other) override;
	virtual bool ReceiveSwirl(AoEHitAbilityDef const& other) override;
	virtual bool ReceiveSwirl(PassiveAoEAbilityDef const& other) override;
	virtual bool ReceiveSwirl(LaserAbilityDef const& other) override;
	virtual bool ReceiveSwirl(AdjacentHitAbilityDef const& other) override;

public:

	AbilityCooldownComponentDef	m_cooldownDef;
	AbilityScalingComponentDef	m_scaling;
	// todo: damage/burn/poison buffs on hit
};