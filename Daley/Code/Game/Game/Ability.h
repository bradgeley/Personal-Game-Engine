// Bradley Christensen - 2022-2026
#pragma once
#include "EntityDebugContext.h"
#include "GameCommon.h"
#include "HitPayload.h"
#include "Engine/Core/Name.h"
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Grid.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"
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
struct AbilityHasteComponentDef;
struct AbilityMultishotComponentDef;
struct AbilityOnHitComponentDef;
struct AbilityPoisonComponentDef;
struct AbilityRenderComponentDef;
struct AbilitySlowComponentDef;
struct AbilityTargetingComponentDef;
struct AoEHitAbilityDef;
struct CTags;
struct EntityDef;
struct LaserAbilityDef;
struct PassiveAoEAbilityDef;
struct ProjectileHitAbilityDef;
struct SystemContext;
struct TowerAbilityRunModifier;
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

	float GetCooldown() const;

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_cooldownSeconds = 0.f;
	float m_accumulatedTime = 0.f;
	float m_attackSpeedIncrease = 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityTargetingComponent
{
public:

	AbilityTargetingComponent() = default;
	AbilityTargetingComponent(AbilityTargetingComponentDef const& def);

	float GetMinRange() const { return m_minRange; } // Min range not affected by multipliers
	float GetMaxRange() const { return m_maxRange * m_rangeMultiplier; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void UpdateCachedTiles(SystemContext const& context, Vec2 const& location);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_minRange = 0.f;
	float m_maxRange = 0.f;
	float m_rangeMultiplier = 1.f;

	uint8_t m_abilityTargetFlags = 0;
	AbilityTargetingMode m_targetingMode = AbilityTargetingMode::ClosestToGoal;

	uint8_t m_needsCacheUpdate = 1;
	Vec2 m_cachedLocation = Vec2::ZeroVector;
	std::vector<IntVec2> m_cachedTilesInRange;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoETargetingComponent : public AbilityTargetingComponent
{
public:

	AbilityAoETargetingComponent() = default;
	AbilityAoETargetingComponent(AbilityTargetingComponentDef const& def);

	bool FindTargets(SystemContext const& context, int maxTargets = -1);

public:

	std::set<EntityID> m_targets;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityPrecisionTargetingComponent : public AbilityTargetingComponent
{
public:
	
	AbilityPrecisionTargetingComponent() = default;
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

	bool CanCrit() const { return m_critChance > 0.f; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

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

	bool IsRelevant() const { return GetMaxDamage() > 0.f; }

	float GetMinDamage() const { return m_minDamage * m_damageMultiplier; }
	float GetMaxDamage() const { return m_maxDamage * m_damageMultiplier; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_minDamage = 0.f;
	float m_maxDamage = 0.f;
	float m_damageMultiplier = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityBurnComponent
{
public:

	AbilityBurnComponent() = default;
	AbilityBurnComponent(AbilityBurnComponentDef const& def);

	bool IsRelevant() const { return GetBurn() > 0.f; }

	float GetBurn() const { return m_burn * m_burnMultiplier; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_burn = 0.f;
	float m_burnMultiplier = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityPoisonComponent
{
public:

	AbilityPoisonComponent() = default;
	AbilityPoisonComponent(AbilityPoisonComponentDef const& def);

	bool IsRelevant() const { return GetPoison() > 0.f; }

	float GetPoison() const { return m_poison * m_poisonMultiplier; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_poison = 0.f;
	float m_poisonMultiplier = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilitySlowComponent
{
public:

	AbilitySlowComponent() = default;
	AbilitySlowComponent(AbilitySlowComponentDef const& def);

	bool IsRelevant() const { return GetDuration() > 0.f; }

	float GetDuration() const { return m_duration * m_durationMultiplier; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_duration = 0.f;
	float m_durationMultiplier = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityHasteComponent
{
public:

	AbilityHasteComponent() = default;
	AbilityHasteComponent(AbilityHasteComponentDef const& def);

	bool IsRelevant() const { return GetDuration() > 0.f; }

	float GetDuration() const { return m_duration * m_durationMultiplier; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_duration = 0.f;
	float m_durationMultiplier = 1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityChainComponent
{
public:

	AbilityChainComponent() = default;
	AbilityChainComponent(AbilityChainComponentDef const& def);

	bool IsRelevant() const { return m_maxChains > 0; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_chainChance	= StaticGameSettings::s_baseChainChance;
	float m_chainDistance = StaticGameSettings::s_baseChainDistance;
	float m_chainPayloadMulti = StaticGameSettings::s_baseChainPayloadMulti;
	int	m_maxChains	= 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityMultishotComponent
{
public:

	AbilityMultishotComponent() = default;
	AbilityMultishotComponent(AbilityMultishotComponentDef const& def);

	bool IsRelevant() const { return m_additionalTargets > 0; }

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	int m_additionalTargets = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityRenderComponent
{
public:

	AbilityRenderComponent() = default;
	AbilityRenderComponent(AbilityRenderComponentDef const& def);

	void AppendDebugString(EntityDebugContext& debugContext) const;

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

	bool IsRelevant() const;

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_radius = 0.f;
	AbilityDamageComponent m_damageOnHit;
	AbilityPoisonComponent m_poisonOnHit;
	AbilityBurnComponent m_burnOnHit;
	AbilitySlowComponent m_slowOnHit;
	AbilityHasteComponent m_hasteOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponent
{
public:

	AbilityAoEEffectComponent() = default;
	AbilityAoEEffectComponent(AbilityAoEEffectComponentDef const& def);

	bool IsRelevant() const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

public:

	Name m_aoeEffectDefName = Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = 0.f;
	AbilityDamageComponent	m_damagePerSecond;
	AbilityPoisonComponent	m_poisonPerSecond;
	AbilityBurnComponent	m_burnPerSecond;
	AbilitySlowComponent	m_slowPerSecond;
	AbilityHasteComponent	m_hastePerSecond;	
	AbilityRenderComponent	m_renderComp;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityOnHitComponent
{
public:

	AbilityOnHitComponent() = default;
	AbilityOnHitComponent(AbilityOnHitComponentDef const& def);

	bool IsRelevant() const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

	void ApplyModifier(TowerAbilityRunModifier const& modifier);

public:

	AbilityDamageComponent		m_damageOnHit;
	AbilityPoisonComponent		m_poisonOnHit;
	AbilityBurnComponent		m_burnOnHit;
	AbilityAoEHitComponent		m_aoeHitOnHit;
	AbilityAoEEffectComponent	m_aoeEffectOnHit;
	AbilitySlowComponent		m_slowOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
struct RolledAoEHitComponent
{
public:

	bool IsRelevant() const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_radius = 0.f;
	HitPayload m_payload;
};



//----------------------------------------------------------------------------------------------------------------------
struct RolledOnHitComponent
{
public:

	bool IsRelevant() const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	HitPayload m_payload;
	RolledAoEHitComponent m_aoeHitOnHit;
	AbilityAoEEffectComponent m_aoeEffectOnHit;
};



//----------------------------------------------------------------------------------------------------------------------
class Ability
{
public:

	Ability(AbilityDef const& def);
	virtual ~Ability() = default;

	virtual void Shutdown(SystemContext const& context);

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation = 1.f) = 0;
	virtual void Render([[maybe_unused]] SystemContext const& context, [[maybe_unused]] Vec2 const& location) const {};
	virtual Ability* DeepCopy() const = 0;
	virtual void CopyTransientDataTo(Ability& other) const = 0;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const = 0;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const;

	virtual void ApplyModifier(TowerAbilityRunModifier const& modifier, CTags const& tags);

public:

	bool m_needsRebuild = true;
	AbilityDef const* m_abilityDef = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class ProjectileHitAbility : public Ability
{
public:

	ProjectileHitAbility() = default;
	explicit ProjectileHitAbility(ProjectileHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation) override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	RolledOnHitComponent RollDamageAndEffects(RandomNumberGenerator& rng) const;

	virtual void ApplyModifier(TowerAbilityRunModifier const& modifier, CTags const& tags) override;

public:

	Name m_projectileDefName = Name::Invalid;
	float m_projSpeed = 1.f;

	AbilityCooldownComponent m_cooldownComp;
	AbilityPrecisionTargetingComponent m_targetingComp;
	AbilityCritComponent m_critComp;
	AbilityOnHitComponent m_onHitComp;
	AbilityChainComponent m_chainComp;
	AbilityMultishotComponent m_multishotComp;
};



//----------------------------------------------------------------------------------------------------------------------
class AoEHitAbility : public Ability
{
public:

	AoEHitAbility() = default;
	explicit AoEHitAbility(AoEHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation) override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	virtual HitPayload RollDamageAndEffects(RandomNumberGenerator& rng) const;

public:

	AbilityCooldownComponent		m_cooldownComp;
	AbilityAoETargetingComponent	m_targetingComp;
	AbilityCritComponent			m_critComp;
	AbilityAoEHitComponent			m_aoeHitComp;
	AbilityAoEEffectComponent		m_aoeEffectComp;
};



//----------------------------------------------------------------------------------------------------------------------
class PassiveAoEAbility : public Ability
{
public:

	PassiveAoEAbility() = default;
	explicit PassiveAoEAbility(PassiveAoEAbilityDef const& def);

	virtual void Shutdown(SystemContext const& context) override;

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation) override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	virtual void ApplyModifier(TowerAbilityRunModifier const& modifier, CTags const& tags) override;

public:

	EntityID m_activeAoEEffect = EntityID::Invalid;
	AbilityAoETargetingComponent	m_targetingComp;
	AbilityAoEEffectComponent		m_aoeEffectComp;
};



//----------------------------------------------------------------------------------------------------------------------
class LaserAbility : public Ability
{
public:

	LaserAbility() = default;
	explicit LaserAbility(LaserAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation) override;
	virtual void Render(SystemContext const& context, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	HitPayload RollDamageAndEffects(float deltaSeconds) const;

public:

	AbilityPrecisionTargetingComponent	m_targetingComp;
	AbilityOnHitComponent				m_onHitComp;
	AbilityRenderComponent				m_renderComp;
	AbilityChainComponent				m_chainComp;
	AbilityMultishotComponent			m_multishotComp;
};