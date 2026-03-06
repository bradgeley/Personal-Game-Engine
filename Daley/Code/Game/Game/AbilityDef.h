// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <optional>
#include <vector>



class Ability;



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCooldownComponentDef
{
	explicit AbilityCooldownComponentDef(void const* xmlElement);

	float m_cooldownSeconds = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityTargetingComponentDef
{
	explicit AbilityTargetingComponentDef(void const* xmlElement);

	float m_minRange = 0.f;
	float m_maxRange = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCritComponentDef
{
	explicit AbilityCritComponentDef(void const* xmlElement);

	float m_critChance = 0.f;
	float m_critMulti = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityDamageComponentDef
{
	explicit AbilityDamageComponentDef(void const* xmlElement);

	float m_minDamage = 0.f;
	float m_maxDamage = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityBurnComponentDef
{
	explicit AbilityBurnComponentDef(void const* xmlElement);

	float m_burn = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityPoisonComponentDef
{
	explicit AbilityPoisonComponentDef(void const* xmlElement);

	float m_poison = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilitySlowComponentDef
{
	explicit AbilitySlowComponentDef(void const* xmlElement);

	float m_duration = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoeHitComponentDef
{
	explicit AbilityAoeHitComponentDef(void const* xmlElement);

	float m_radius = 0.f;
	std::optional<AbilityDamageComponentDef>	m_damageOnHit;
	std::optional<AbilityPoisonComponentDef>	m_poisonOnHit;
	std::optional<AbilityBurnComponentDef>		m_burnOnHit;
	std::optional<AbilitySlowComponentDef>		m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoeEffectComponentDef
{
	explicit AbilityAoeEffectComponentDef(void const* xmlElement);

	float m_radius = 0.f;
	float m_durationSeconds = 0.f;
	std::optional<AbilityDamageComponentDef>	m_damagePerSecond;
	std::optional<AbilityPoisonComponentDef>	m_poisonPerSecond;
	std::optional<AbilityBurnComponentDef>		m_burnPerSecond;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityOnHitComponentDef
{
	explicit AbilityOnHitComponentDef(void const* xmlElement);

	std::optional<AbilityDamageComponentDef>	m_damageOnHit;
	std::optional<AbilityPoisonComponentDef>	m_poisonOnHit;
	std::optional<AbilityBurnComponentDef>		m_burnOnHit;
	std::optional<AbilityAoeHitComponentDef>	m_aoeHitOnHit;
	std::optional<AbilityAoeEffectComponentDef>	m_aoeEffectOnHit;
	std::optional<AbilitySlowComponentDef>		m_slowOnHit;
};




//----------------------------------------------------------------------------------------------------------------------
struct AbilityDef
{
public:

	virtual ~AbilityDef() = default;
	virtual Ability* MakeAbilityInstance() const = 0;

public:

	static void LoadFromXML();
	static void Shutdown();
	static AbilityDef const* GetAbilityDef(uint8_t id);
	static AbilityDef const* GetAbilityDef(Name name);
	static int GetAbilityDefID(Name name);

private:

	static std::vector<AbilityDef*> s_abilityDefs;

public:

	Name m_name = Name::Invalid;
};



//----------------------------------------------------------------------------------------------------------------------
struct ProjectileHitAbilityDef : public AbilityDef
{
public:

	explicit ProjectileHitAbilityDef(void const* xmlElement);

	virtual Ability* MakeAbilityInstance() const override;

public:

	Name m_projectileDefName			= Name::Invalid;
	float m_projSpeed	= 1.f;

	std::optional<AbilityCooldownComponentDef>		m_cooldownDef;
	std::optional<AbilityTargetingComponentDef>		m_targetingDef;
	std::optional<AbilityCritComponentDef>			m_critDef;
	std::optional<AbilityOnHitComponentDef>			m_onHitDef;
};