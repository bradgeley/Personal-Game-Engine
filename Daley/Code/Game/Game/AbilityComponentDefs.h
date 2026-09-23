// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCooldownComponentDef
{
	AbilityCooldownComponentDef() = default;
	explicit AbilityCooldownComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_cooldownSeconds = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityTargetingComponentDef
{
	AbilityTargetingComponentDef() = default;
	explicit AbilityTargetingComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_minRange = 0.f;
	float m_maxRange = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCritComponentDef
{
	AbilityCritComponentDef() = default;
	explicit AbilityCritComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_critChance = 0.f;
	float m_critMulti = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityDamageComponentDef
{
	AbilityDamageComponentDef() = default;
	explicit AbilityDamageComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float GetAverageDamage() const { return (m_minDamage + m_maxDamage) * 0.5f; }

	float m_minDamage = 0.f;
	float m_maxDamage = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityBurnComponentDef
{
	AbilityBurnComponentDef() = default;
	explicit AbilityBurnComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_burn = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityPoisonComponentDef
{
	AbilityPoisonComponentDef() = default;
	explicit AbilityPoisonComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_poison = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilitySlowComponentDef
{
	AbilitySlowComponentDef() = default;
	explicit AbilitySlowComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_duration = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityHasteComponentDef
{
	AbilityHasteComponentDef() = default;
	explicit AbilityHasteComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_duration = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityChainComponentDef
{
	AbilityChainComponentDef() = default;
	explicit AbilityChainComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float	m_chainChance = 1.f;
	float	m_chainDistance = 3.f;
	float	m_chainPayloadMulti = 1.f;
	int		m_maxChains = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityMultishotComponentDef
{
	AbilityMultishotComponentDef() = default;
	explicit AbilityMultishotComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	int	m_additionalTargets = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityRenderComponentDef
{
	AbilityRenderComponentDef() = default;
	explicit AbilityRenderComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	Rgba8 m_tint = Rgba8::White;
	float m_depth = 0.f;
	float m_renderDuration = -1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEHitComponentDef
{
	AbilityAoEHitComponentDef() = default;
	explicit AbilityAoEHitComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_radius = 0.f;
	AbilityDamageComponentDef	m_damageOnHit;
	AbilityPoisonComponentDef	m_poisonOnHit;
	AbilityBurnComponentDef		m_burnOnHit;
	AbilitySlowComponentDef		m_slowOnHit;
	AbilityHasteComponentDef	m_hasteOnHit;
	AbilityRenderComponentDef	m_renderDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponentDef
{
	AbilityAoEEffectComponentDef() = default;
	explicit AbilityAoEEffectComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	Name m_aoeEffectDefName	= Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = -1.f; // negative == infinite
	AbilityDamageComponentDef	m_damagePerSecond;
	AbilityPoisonComponentDef	m_poisonPerSecond;
	AbilityBurnComponentDef		m_burnPerSecond;
	AbilitySlowComponentDef		m_slowPerSecond;
	AbilityHasteComponentDef	m_hastePerSecond;
	AbilityRenderComponentDef	m_renderDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityOnHitComponentDef
{
	AbilityOnHitComponentDef() = default;
	explicit AbilityOnHitComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	AbilityDamageComponentDef		m_damageOnHit;
	AbilityPoisonComponentDef		m_poisonOnHit;
	AbilityBurnComponentDef			m_burnOnHit;
	AbilitySlowComponentDef			m_slowOnHit;
	AbilityAoEHitComponentDef		m_aoeHitOnHit;
	AbilityAoEEffectComponentDef	m_aoeEffectOnHit;
};