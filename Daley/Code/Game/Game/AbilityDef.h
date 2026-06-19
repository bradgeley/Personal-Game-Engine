// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"
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
struct AbilityAoEHitComponentDef
{
	explicit AbilityAoEHitComponentDef(void const* xmlElement);

	float m_radius = 0.f;
	std::optional<AbilityDamageComponentDef>	m_damageOnHit;
	std::optional<AbilityPoisonComponentDef>	m_poisonOnHit;
	std::optional<AbilityBurnComponentDef>		m_burnOnHit;
	std::optional<AbilitySlowComponentDef>		m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityRenderComponentDef
{
	explicit AbilityRenderComponentDef(void const* xmlElement);

	Rgba8 m_tint	= Rgba8::White;
	float m_depth	= 0.f;
};




//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponentDef
{
	explicit AbilityAoEEffectComponentDef(void const* xmlElement);

	Name m_aoeEffectDefName	= Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = -1.f; // negative == infinite
	std::optional<AbilityDamageComponentDef>	m_damagePerSecond;
	std::optional<AbilityPoisonComponentDef>	m_poisonPerSecond;
	std::optional<AbilityBurnComponentDef>		m_burnPerSecond;
	std::optional<AbilitySlowComponentDef>		m_slowPerSecond;

	// Rendering
	std::optional<AbilityRenderComponentDef>	m_renderDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityOnHitComponentDef
{
	explicit AbilityOnHitComponentDef(void const* xmlElement);

	std::optional<AbilityDamageComponentDef>	m_damageOnHit;
	std::optional<AbilityPoisonComponentDef>	m_poisonOnHit;
	std::optional<AbilityBurnComponentDef>		m_burnOnHit;
	std::optional<AbilityAoEHitComponentDef>	m_aoeHitOnHit;
	std::optional<AbilityAoEEffectComponentDef>	m_aoeEffectOnHit;
	std::optional<AbilitySlowComponentDef>		m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityDef
{
public:

	virtual ~AbilityDef() = default;
	AbilityDef(void const* xmlElement);
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



//----------------------------------------------------------------------------------------------------------------------
struct AoEHitAbilityDef : public AbilityDef
{
public:

	explicit AoEHitAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;

public:

	std::optional<AbilityCooldownComponentDef>		m_cooldownDef;
	std::optional<AbilityTargetingComponentDef>		m_targetingDef;
	std::optional<AbilityCritComponentDef>			m_critDef;
	std::optional<AbilityAoEHitComponentDef>		m_aoeHitDef;
	std::optional<AbilityAoEEffectComponentDef>		m_aoeEffectDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct PassiveAoEAbilityDef : public AbilityDef
{
public:

	explicit PassiveAoEAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;

public:

	std::optional<AbilityTargetingComponentDef> m_targetingDef;
	std::optional<AbilityAoEEffectComponentDef>	m_aoeEffectDef;
};