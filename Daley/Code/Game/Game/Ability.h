// Bradley Christensen - 2022-2026
#pragma once
#include "HitPayload.h"
#include "Engine/Core/Name.h"
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Grid.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <optional>
#include <set>
#include <vector>



struct AbilityAoEEffectComponentDef;
struct AbilityAoEHitComponentDef;
struct AbilityBurnComponentDef;
struct AbilityChainComponentDef;
struct AbilityCooldownComponentDef;
struct AbilityCritComponentDef;
struct AbilityDamageComponentDef;
struct AbilityDef;
struct AbilityMultishotComponentDef;
struct AbilityOnHitComponentDef;
struct AbilityPoisonComponentDef;
struct AbilityRenderComponentDef;
struct AbilitySlowComponentDef;
struct AbilityTargetingComponentDef;
struct AoEHitAbilityDef;
struct EntityDef;
struct LaserAbilityDef;
struct PassiveAoEAbilityDef;
struct ProjectileHitAbilityDef;
struct SystemContext;
struct Vec2;
class RandomNumberGenerator;
class VertexBuffer;



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

	bool NeedsCacheUpdate(Vec2 const& location) const;
	bool UpdateCachedTiles(SystemContext const& context, Vec2 const& location);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_minRange = 0.f;
	float m_maxRange = 0.f;

	AbilityTargetingMode m_targetingMode = AbilityTargetingMode::ClosestToGoal;

	float m_minRangeAtTimeOfCache = -1.f;
	float m_maxRangeAtTimeOfCache = -1.f;
	Vec2 m_locationAtTimeOfCache = Vec2::ZeroVector;
	std::vector<IntVec2> m_cachedPathTilesInRange;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoETargetingComponent : public AbilityTargetingComponent
{
public:

	AbilityAoETargetingComponent(AbilityTargetingComponentDef const& def);

	bool FindTargets(SystemContext const& context, int maxTargets = -1);

public:

	std::set<EntityID> m_targets;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityPrecisionTargetingComponent : public AbilityTargetingComponent
{
public:

	AbilityPrecisionTargetingComponent(AbilityTargetingComponentDef const& def);

	bool FindTargets(SystemContext const& context, int maxTargets = 1, int maxChains = 0, float maxChainDistance = 1.f);
	EntityID FindChainTarget(SystemContext const& context, Vec2 const& pos, float maxDistance);

public:

	Grid<EntityID> m_targetChains;
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
struct AbilityChainComponent
{
public:

	AbilityChainComponent() = default;
	AbilityChainComponent(AbilityChainComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_chainChance	= 0.f;
	float m_chainDistance = 3.f;
	float m_chainPayloadMulti = 1.f;
	int	m_maxChains	= 0;
	int	m_remainingChains = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityMultishotComponent
{
public:

	AbilityMultishotComponent() = default;
	AbilityMultishotComponent(AbilityMultishotComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	int m_additionalTargets = 1;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityRenderComponent
{
public:

	AbilityRenderComponent() = default;
	AbilityRenderComponent(AbilityRenderComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	Rgba8 m_tint = Rgba8::White;
	float m_depth = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEHitComponent
{
public:

	AbilityAoEHitComponent() = default;
	AbilityAoEHitComponent(AbilityAoEHitComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	float m_radius = 0.f;
	std::optional<AbilityDamageComponent>	m_damageOnHit;
	std::optional<AbilityPoisonComponent>	m_poisonOnHit;
	std::optional<AbilityBurnComponent>		m_burnOnHit;
	std::optional<AbilitySlowComponent>		m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponent
{
public:

	AbilityAoEEffectComponent() = default;
	AbilityAoEEffectComponent(AbilityAoEEffectComponentDef const& def);

	void AppendDebugString(std::string& out_string) const;

public:

	Name m_aoeEffectDefName = Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = 0.f;
	std::optional<AbilityDamageComponent>	m_damagePerSecond;
	std::optional<AbilityPoisonComponent>	m_poisonPerSecond;
	std::optional<AbilityBurnComponent>		m_burnPerSecond;
	std::optional<AbilitySlowComponent>		m_slowPerSecond;
	std::optional<AbilityRenderComponent>	m_renderComp;
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
	std::optional<AbilityAoEHitComponent>		m_aoeHitOnHit;
	std::optional<AbilityAoEEffectComponent>	m_aoeEffectOnHit;
	std::optional<AbilitySlowComponent>			m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct RolledAoEHitComponent
{
public:

	void AppendDebugString(std::string& out_string) const;

public:

	float m_radius = 0.f;
	HitPayload m_payload;
};



//----------------------------------------------------------------------------------------------------------------------
struct RolledOnHitComponent
{
public:

	void AppendDebugString(std::string& out_string) const;

public:

	HitPayload m_payload;
	std::optional<RolledAoEHitComponent>		m_aoeHitOnHit;
	std::optional<AbilityAoEEffectComponent>	m_aoeEffectOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
class Ability
{
public:

	Ability(AbilityDef const& def);
	virtual ~Ability() = default;

	virtual void Update(SystemContext const& context, Vec2 const& location) = 0;
	virtual void Render([[maybe_unused]] SystemContext const& context, [[maybe_unused]] Vec2 const& location) const {};
	virtual Ability* DeepCopy() const = 0;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const = 0;
	virtual void AppendDebugString(std::string& out_string) const;

public:

	AbilityDef const* m_abilityDef = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class ProjectileHitAbility : public Ability
{
public:

	ProjectileHitAbility() = default;
	explicit ProjectileHitAbility(ProjectileHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location) override;
	virtual Ability* DeepCopy() const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(std::string& out_string) const override;

	RolledOnHitComponent RollDamageAndEffects(RandomNumberGenerator& rng) const;

public:

	Name m_projectileDefName = Name::Invalid;
	float m_projSpeed = 1.f;

	std::optional<AbilityCooldownComponent>				m_cooldownComp;
	std::optional<AbilityPrecisionTargetingComponent>	m_targetingComp;
	std::optional<AbilityCritComponent>					m_critComp;
	std::optional<AbilityOnHitComponent>				m_onHitComp;
	std::optional<AbilityChainComponent>				m_chainComp;
	std::optional<AbilityMultishotComponent>			m_multishotComp;
};



//----------------------------------------------------------------------------------------------------------------------
class AoEHitAbility : public Ability
{
public:

	AoEHitAbility() = default;
	explicit AoEHitAbility(AoEHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location) override;
	virtual Ability* DeepCopy() const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(std::string& out_string) const override;

	virtual HitPayload RollDamageAndEffects(RandomNumberGenerator& rng) const;

public:

	std::optional<AbilityCooldownComponent>		m_cooldownComp;
	std::optional<AbilityAoETargetingComponent>	m_targetingComp;
	std::optional<AbilityCritComponent>			m_critComp;
	std::optional<AbilityAoEHitComponent>		m_aoeHitComp;
	std::optional<AbilityAoEEffectComponent>	m_aoeEffectComp;
};



//----------------------------------------------------------------------------------------------------------------------
class PassiveAoEAbility : public Ability
{
public:

	PassiveAoEAbility() = default;
	explicit PassiveAoEAbility(PassiveAoEAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location) override;
	virtual Ability* DeepCopy() const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(std::string& out_string) const override;

public:

	EntityID m_activeAoEEffect = EntityID::Invalid;
	std::optional<AbilityAoETargetingComponent>	m_targetingComp;
	std::optional<AbilityAoEEffectComponent>	m_aoeEffectComp;
};



//----------------------------------------------------------------------------------------------------------------------
class LaserAbility : public Ability
{
public:

	LaserAbility() = default;
	explicit LaserAbility(LaserAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location) override;
	virtual void Render(SystemContext const& context, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(std::string& out_string) const override;

	HitPayload RollDamageAndEffects(float deltaSeconds) const;

public:

	std::optional<AbilityPrecisionTargetingComponent>	m_targetingComp;
	std::optional<AbilityOnHitComponent>				m_onHitComp;
	std::optional<AbilityRenderComponent>				m_renderComp;
	std::optional<AbilityChainComponent>				m_chainComp;
	std::optional<AbilityMultishotComponent>			m_multishotComp;
};