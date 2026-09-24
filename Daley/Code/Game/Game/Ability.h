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
#include <optional>
#include <set>
#include <vector>



struct AbilityAoEEffectComponentDef;
struct AbilityAoEHitComponentDef;
struct AbilityChainComponentDef;
struct AbilityCooldownComponentDef;
struct AbilityCritComponentDef;
struct AbilityScalingComponentDef;
struct AbilityDef;
struct AbilityMultishotComponentDef;
struct AbilityOnHitComponentDef;
struct AbilityRenderComponentDef;
struct AbilityTargetingComponentDef;
struct AdjacentHitAbilityDef;
struct AoEHitAbilityDef;
struct CPlaceable;
struct CTags;
struct EntityDef;
struct LaserAbilityDef;
struct PassiveAoEAbilityDef;
struct ProjectileHitAbilityDef;
struct SystemContext;
struct FlavorAbilityRunModifier;
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

	void AppendDebugString(EntityDebugContext& debugContext) const;

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

	float GetMinRange() const { return m_minRange; }
	float GetMaxRange() const { return m_maxRange; }

	void UpdateCachedTiles(SystemContext const& context, Vec2 const& location);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_minRange = 0.f;
	float m_maxRange = 0.f;

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

	std::vector<EntityID> m_targets;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAdjacentTargetingComponent
{
public:

	AbilityAdjacentTargetingComponent() = default;

	bool FindTargets(SystemContext const& context, EntityID owner);

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

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_critChance = 0.f;
	float m_critMulti = 0.f; // Additive with the base 2x crit multiplier
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityScalingComponent
{
public:

	AbilityScalingComponent() = default;
	AbilityScalingComponent(AbilityScalingComponentDef const& def);

	bool IsRelevant() const { return m_physical > 0.f || m_burn > 0.f || m_poison > 0.f || m_slow > 0.f || m_haste > 0.f; }

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_physical = 0.f;
	float m_burn = 0.f;
	float m_poison = 0.f;
	float m_slow = 0.f;
	float m_haste = 0.f;
};


//----------------------------------------------------------------------------------------------------------------------
struct AbilityChainComponent
{
public:

	AbilityChainComponent() = default;
	AbilityChainComponent(AbilityChainComponentDef const& def);

	bool IsRelevant() const { return m_maxChains > 0; }

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
	float m_renderDuration = -1.f;
	float m_renderDurationRemaining = -1.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEHitComponent
{
public:

	AbilityAoEHitComponent() = default;
	AbilityAoEHitComponent(AbilityAoEHitComponentDef const& def);

	bool IsRelevant() const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_radius = 0.f;
	AbilityScalingComponent m_scaling;
	std::optional<AbilityRenderComponent> m_renderComp;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponent
{
public:

	AbilityAoEEffectComponent() = default;
	AbilityAoEEffectComponent(AbilityAoEEffectComponentDef const& def);

	bool IsRelevant() const;
	float GetRadius() const { return m_radius * m_radiusMultiplier; }
	float GetDuration() const { return m_durationSeconds * m_durationMultiplier; }

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	Name m_aoeEffectDefName = Name::Invalid;
	float m_radius = 0.f;
	float m_radiusMultiplier = 1.f;
	float m_durationSeconds = 0.f;
	float m_durationMultiplier = 1.f;
	AbilityScalingComponent	m_scaling;
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

public:

	AbilityScalingComponent		m_scaling;
	AbilityAoEHitComponent		m_aoeHitOnHit;
	AbilityAoEEffectComponent	m_aoeEffectOnHit;
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

	virtual void Initialize(SystemContext const& context, EntityID ownerEntityID);
	virtual void Shutdown(SystemContext const& context);

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation = 1.f) = 0;
	virtual void Render([[maybe_unused]] SystemContext const& context, [[maybe_unused]] Vec2 const& location) const {};
	virtual Ability* DeepCopy() const = 0;
	virtual void CopyTransientDataTo(Ability& other) const = 0;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, Vec2 const& location) const = 0;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	bool m_needsRebuild = true;
	EntityID m_owner = EntityID::Invalid;
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
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	RolledOnHitComponent RollDamageAndEffects(RandomNumberGenerator& rng) const;

public:

	Name m_projectileDefName = Name::Invalid;
	float m_projSpeed = 1.f;

	AbilityCooldownComponent m_cooldownComp;
	AbilityPrecisionTargetingComponent m_targetingComp;
	AbilityCritComponent m_critComp;
	AbilityChainComponent m_chainComp;
	AbilityMultishotComponent m_multishotComp;
	AbilityOnHitComponent m_onHitComp;
};



//----------------------------------------------------------------------------------------------------------------------
class AoEHitAbility : public Ability
{
public:

	AoEHitAbility() = default;
	explicit AoEHitAbility(AoEHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation) override;
	virtual void Render(SystemContext const& context, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, Vec2 const& location) const override;
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
	virtual void Render(SystemContext const& context, Vec2 const& location) const override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	virtual HitPayload GetDotPayload(float deltaSeconds) const;

public:

	AbilityAoETargetingComponent	m_targetingComp;
	AbilityAoEEffectComponent		m_aoeEffectComp;
};



//----------------------------------------------------------------------------------------------------------------------
class AdjacentHitAbility : public Ability
{
public:

	AdjacentHitAbility() = default;
	explicit AdjacentHitAbility(AdjacentHitAbilityDef const& def);

	virtual void Update(SystemContext const& context, Vec2 const& location, float timeDilation) override;
	virtual Ability* DeepCopy() const override;
	virtual void CopyTransientDataTo(Ability& other) const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

public:

	AbilityCooldownComponent			m_cooldownComp;
	AbilityAdjacentTargetingComponent	m_targetingComp;
	AbilityScalingComponent				m_scaling;
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
	virtual void AddDebugVerts(VertexBuffer& out_vbo, CPlaceable const& placeable, Vec2 const& location) const override;
	virtual void AppendDebugString(EntityDebugContext& debugContext) const override;

	HitPayload RollDamageAndEffects(float deltaSeconds) const;

public:

	AbilityPrecisionTargetingComponent	m_targetingComp;
	AbilityCooldownComponent 			m_cooldownComp; // Cooldown is 0 for lasers, this is just a receptacle for attack speed modifiers
	AbilityCritComponent 				m_critComp;
	AbilityOnHitComponent				m_onHitComp;
	AbilityRenderComponent				m_renderComp;
	AbilityChainComponent				m_chainComp;
	AbilityMultishotComponent			m_multishotComp;
};