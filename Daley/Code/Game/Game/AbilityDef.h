// Bradley Christensen - 2022-2026
#pragma once
#include "GameCommon.h"
#include "Engine/Core/Name.h"
#include "Engine/Renderer/Rgba8.h"
#include <optional>
#include <vector>



class Ability;



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCooldownComponentDef
{
	explicit AbilityCooldownComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_cooldownSeconds = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityTargetingComponentDef
{
	explicit AbilityTargetingComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_minRange = 0.f;
	float m_maxRange = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCritComponentDef
{
	explicit AbilityCritComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_critChance = 0.f;
	float m_critMulti = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityDamageComponentDef
{
	explicit AbilityDamageComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_minDamage = 0.f;
	float m_maxDamage = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityBurnComponentDef
{
	explicit AbilityBurnComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_burn = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityPoisonComponentDef
{
	explicit AbilityPoisonComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_poison = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilitySlowComponentDef
{
	explicit AbilitySlowComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_duration = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityHasteComponentDef
{
	explicit AbilityHasteComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_duration = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityChainComponentDef
{
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
	explicit AbilityMultishotComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	int	m_additionalTargets = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityRenderComponentDef
{
	explicit AbilityRenderComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	Rgba8 m_tint = Rgba8::White;
	float m_depth = 0.f;
	float m_renderDuration = -1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEHitComponentDef
{
	explicit AbilityAoEHitComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	float m_radius = 0.f;
	std::optional<AbilityDamageComponentDef>	m_damageOnHit;
	std::optional<AbilityPoisonComponentDef>	m_poisonOnHit;
	std::optional<AbilityBurnComponentDef>		m_burnOnHit;
	std::optional<AbilitySlowComponentDef>		m_slowOnHit;
	std::optional<AbilityHasteComponentDef>		m_hasteOnHit;
	std::optional<AbilityRenderComponentDef>	m_renderDef;
};




//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponentDef
{
	explicit AbilityAoEEffectComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	Name m_aoeEffectDefName	= Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = -1.f; // negative == infinite
	std::optional<AbilityDamageComponentDef>	m_damagePerSecond;
	std::optional<AbilityPoisonComponentDef>	m_poisonPerSecond;
	std::optional<AbilityBurnComponentDef>		m_burnPerSecond;
	std::optional<AbilitySlowComponentDef>		m_slowPerSecond;
	std::optional<AbilityHasteComponentDef>		m_hastePerSecond;

	// Rendering
	std::optional<AbilityRenderComponentDef>	m_renderDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityOnHitComponentDef
{
	explicit AbilityOnHitComponentDef(void const* xmlElement);
	void WriteToXmlDoc(void* xmlDoc, void* parentElem);

	std::optional<AbilityDamageComponentDef>	m_damageOnHit;
	std::optional<AbilityPoisonComponentDef>	m_poisonOnHit;
	std::optional<AbilityBurnComponentDef>		m_burnOnHit;
	std::optional<AbilitySlowComponentDef>		m_slowOnHit;
	std::optional<AbilityAoEHitComponentDef>	m_aoeHitOnHit;
	std::optional<AbilityAoEEffectComponentDef>	m_aoeEffectOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityDef
{
public:

	virtual ~AbilityDef() = default;
	AbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const = 0;
	virtual AbilityDef* Copy() const = 0;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) = 0;

public:

	static void LoadFromXML(Name filepath);
	static void SaveToXML(Name filepath);
	static void Shutdown();
	static AbilityDef const* GetAbilityDef(uint8_t id);
	static AbilityDef const* GetAbilityDef(Name name);
	static int GetAbilityDefID(Name name);
	static std::vector<AbilityDef*> const& GetAllAbilityDefs();

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
	virtual AbilityDef* Copy() const override;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

public:

	Name m_projectileDefName			= Name::Invalid;
	float m_projSpeed					= 1.f;

	std::optional<AbilityCooldownComponentDef>		m_cooldownDef;
	std::optional<AbilityTargetingComponentDef>		m_targetingDef;
	std::optional<AbilityCritComponentDef>			m_critDef;
	std::optional<AbilityChainComponentDef>			m_chainDef;
	std::optional<AbilityMultishotComponentDef>		m_multishotDef;
	std::optional<AbilityOnHitComponentDef>			m_onHitDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AoEHitAbilityDef : public AbilityDef
{
public:

	explicit AoEHitAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

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
	virtual AbilityDef* Copy() const override;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

public:

	std::optional<AbilityTargetingComponentDef> m_targetingDef;
	std::optional<AbilityAoEEffectComponentDef>	m_aoeEffectDef;
};



//----------------------------------------------------------------------------------------------------------------------
struct AdjacentHitAbilityDef : public AbilityDef
{
public:

	explicit AdjacentHitAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

public:

	std::optional<AbilityCooldownComponentDef> m_cooldownDef;
	std::optional<AbilityHasteComponentDef> m_hasteOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct LaserAbilityDef : public AbilityDef
{
public:

	explicit LaserAbilityDef(void const* xmlElement);
	virtual Ability* MakeAbilityInstance() const override;
	virtual AbilityDef* Copy() const override;
	virtual void WriteToXmlDoc(void* xmlDoc, void* rootElement) override;

public:

	std::optional<AbilityTargetingComponentDef>		m_targetingDef;
	std::optional<AbilityOnHitComponentDef>			m_onHitDef;
	std::optional<AbilityRenderComponentDef>		m_renderDef;
	std::optional<AbilityChainComponentDef>			m_chainDef;
	std::optional<AbilityMultishotComponentDef>		m_multishotDef;
};