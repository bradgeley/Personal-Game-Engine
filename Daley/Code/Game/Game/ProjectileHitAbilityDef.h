// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityComponentDefs.h"
#include "AbilityDef.h"
#include "Engine/Core/Name.h"



//----------------------------------------------------------------------------------------------------------------------
struct ProjectileHitAbilityDef : public AbilityDef
{
public:

	explicit ProjectileHitAbilityDef(void const* xmlElement);
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

	Name m_projectileDefName = Name::Invalid;
	float m_projSpeed = 1.f;
	AbilityCooldownComponentDef		m_cooldownDef;
	AbilityTargetingComponentDef	m_targetingDef;
	AbilityCritComponentDef			m_critDef;
	AbilityChainComponentDef		m_chainDef;
	AbilityMultishotComponentDef	m_multishotDef;
	AbilityOnHitComponentDef		m_onHitDef;
};