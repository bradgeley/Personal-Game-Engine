// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Math/IntVec2.h"
#include <optional>
#include <vector>



class VertexBuffer;
struct EntityDef;
struct ProjectileHitAbilityDef;
struct Vec2;
struct AbilityDef;
struct AbilityCooldownComponentDef;
struct AbilityTargetingComponentDef;
struct AbilityCritComponentDef;
struct AbilityDamageComponentDef;
struct AbilityBurnComponentDef;
struct AbilityPoisonComponentDef;
struct AbilityAoeHitComponentDef;
struct AbilityAoeEffectComponentDef;
struct AbilityOnHitComponentDef;
struct AbilitySlowComponentDef;



//----------------------------------------------------------------------------------------------------------------------
enum class AbilityTargetingMode
{
	ClosestToGoal,
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCooldownComponent
{
public:

	AbilityCooldownComponent() = default;
	AbilityCooldownComponent(AbilityCooldownComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_cooldownSeconds = 0.f;
	float m_accumulatedTime = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityTargetingComponent
{
public:

	AbilityTargetingComponent() = default;
	AbilityTargetingComponent(AbilityTargetingComponentDef const& def);

	bool HasRangeChanged() const;

	void AppendDebugString(std::string& out_string) const;

public:

	float m_minRange = 0.f;
	float m_maxRange = 0.f;
	AbilityTargetingMode m_targetingMode = AbilityTargetingMode::ClosestToGoal;

	float m_minRangeAtTimeOfCache = -1.f;
	float m_maxRangeAtTimeOfCache = -1.f;
	std::vector<IntVec2> m_cachedPathTilesInRange;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityCritComponent
{
public:

	AbilityCritComponent() = default;
	AbilityCritComponent(AbilityCritComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_critChance = 0.f;
	float m_critMulti = 0.f; // Additive with the base 2x crit multiplier

	bool m_didCrit = false;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityDamageComponent
{
public:

	AbilityDamageComponent() = default;
	AbilityDamageComponent(AbilityDamageComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_minDamage = 0.f;
	float m_maxDamage = 0.f;

	float m_damageDone = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityBurnComponent
{
public:

	AbilityBurnComponent() = default;
	AbilityBurnComponent(AbilityBurnComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_burn = 0.f;

	float m_burnDone = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityPoisonComponent
{
public:

	AbilityPoisonComponent() = default;
	AbilityPoisonComponent(AbilityPoisonComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_poison = 0.f;

	float m_poisonDone = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilitySlowComponent
{
public:

	AbilitySlowComponent() = default;
	AbilitySlowComponent(AbilitySlowComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_duration = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoeHitComponent
{
public:

	AbilityAoeHitComponent() = default;
	AbilityAoeHitComponent(AbilityAoeHitComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_radius = 0.f;
	std::optional<AbilityDamageComponent>	m_damageOnHit;
	std::optional<AbilityPoisonComponent>	m_poisonOnHit;
	std::optional<AbilityBurnComponent>		m_burnOnHit;
	std::optional<AbilitySlowComponent>		m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoeEffectComponent
{
public:

	AbilityAoeEffectComponent() = default;
	AbilityAoeEffectComponent(AbilityAoeEffectComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_radius = 0.f;
	float m_durationSeconds = 0.f;
	std::optional<AbilityDamageComponent>	m_damagePerSecond;
	std::optional<AbilityPoisonComponent>	m_poisonPerSecond;
	std::optional<AbilityBurnComponent>		m_burnPerSecond;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityOnHitComponent
{
public:

	AbilityOnHitComponent() = default;
	AbilityOnHitComponent(AbilityOnHitComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	std::optional<AbilityDamageComponent>		m_damageOnHit;
	std::optional<AbilityPoisonComponent>		m_poisonOnHit;
	std::optional<AbilityBurnComponent>			m_burnOnHit;
	std::optional<AbilityAoeHitComponent>		m_aoeHitOnHit;
	std::optional<AbilityAoeEffectComponent>	m_aoeEffectOnHit;
	std::optional<AbilitySlowComponent>			m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
class Ability
{
public:

	Ability() = default;
	virtual ~Ability() = default;

	virtual void Update(float deltaSeconds, Vec2 const& location) = 0;
	virtual Ability* DeepCopy() const = 0;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const = 0;
	virtual void AppendDebugString(std::string& out_string) const = 0;

public:

	AbilityDef const* m_abilityDef = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class ProjectileHitAbility : public Ability
{
public:

	ProjectileHitAbility() = default;
	explicit ProjectileHitAbility(ProjectileHitAbilityDef const& def);

	virtual void Update(float deltaSeconds, Vec2 const& location) override;
	virtual Ability* DeepCopy() const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(std::string& out_string) const override;

public:

	Name m_projectileDefName = Name::Invalid;
	float m_projSpeed = 1.f;

	std::optional<AbilityCooldownComponent>		m_cooldownComp;
	std::optional<AbilityTargetingComponent>	m_targetingComp;
	std::optional<AbilityCritComponent>			m_critComp;
	std::optional<AbilityOnHitComponent>		m_onHitComp;
};