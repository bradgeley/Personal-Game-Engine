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



struct CAbility;
struct CTime;
struct AbilityAoEEffectComponentDef;
struct AbilityAoEHitComponentDef;
struct AbilityChainComponentDef;
struct AbilityCooldownComponentDef;
struct AbilityCritComponentDef;
struct AbilityMultishotComponentDef;
struct AbilityOnHitComponentDef;
struct AbilityRenderComponentDef;
struct AbilityScalingComponentDef;
struct AbilityTargetingComponentDef;
struct SystemContext;



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

	float GetCooldown(CAbility const& ability) const;

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

	virtual float GetMaxRange(CAbility const& ability) const;

	void UpdateCachedTiles(SystemContext const& context, CAbility const& ability, Vec2 const& location);

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_maxRange = 0.f;

	AbilityTargetingMode m_targetingMode = AbilityTargetingMode::ClosestToGoal;

	float m_cachedMaxRange = 0.f;
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

	virtual float GetMaxRange(CAbility const& ability) const override;

	bool FindTargets(SystemContext const& context, CAbility const& ability, int maxTargets = -1);

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

	float GetCritChance(CAbility const& ability) const;
	float GetCritMultiplier(CAbility const& ability) const;

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
	HitPayload CalculateHitPayload(CAbility const& ability, bool didCrit = false, float critMultiplier = 1.f) const;
	HitPayload CalculateDotPayload(CAbility const& ability, float deltaSeconds) const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	// Scaling values are multiplied by the entity's ability attributes to determine the final payload
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

	int GetMaxChains(CAbility const& ability) const;
	float GetChainDistance(CAbility const& ability) const;
	float GetChainPayloadMulti(CAbility const& ability) const;
	float GetChainChance(CAbility const& ability) const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_chainChance = StaticGameSettings::s_baseChainChance;
	float m_chainDistance = StaticGameSettings::s_baseChainDistance;
	float m_chainPayloadMulti = StaticGameSettings::s_baseChainPayloadMulti;
	int	m_maxChains = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityMultishotComponent
{
public:

	AbilityMultishotComponent() = default;
	AbilityMultishotComponent(AbilityMultishotComponentDef const& def);

	int GetAdditionalTargets(CAbility const& ability) const;

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
struct AbilityAoEHitComponent
{
public:

	AbilityAoEHitComponent() = default;
	AbilityAoEHitComponent(AbilityAoEHitComponentDef const& def);

	bool IsRelevant() const;
	float GetRadius(CAbility const& ability) const;
	RolledAoEHitComponent CalculateSplashPayload(CAbility const& ability, bool didCrit, float critMultiplier) const;
	RolledAoEHitComponent CalculateDotPayload(CAbility const& ability, float deltaSeconds) const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	float m_radius = 0.f;
	AbilityScalingComponent m_scaling;
	std::optional<AbilityRenderComponent> m_renderComp;
};



//----------------------------------------------------------------------------------------------------------------------
struct RolledAoEEffectComponent
{
public:

	bool IsRelevant() const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	Name m_aoeEffectDefName = Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = 0.f;
	HitPayload m_payload;
	AbilityRenderComponent m_renderComp;
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAoEEffectComponent
{
public:

	AbilityAoEEffectComponent() = default;
	AbilityAoEEffectComponent(AbilityAoEEffectComponentDef const& def);

	bool IsRelevant() const;
	float GetRadius(CAbility const& ability) const;
	float GetDuration(CAbility const& ability) const;
	RolledAoEEffectComponent CalculatePuddlePayload(CAbility const& ability) const;
	RolledAoEEffectComponent CalculateDotPayload(CAbility const& ability, float deltaSeconds) const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

public:

	Name m_aoeEffectDefName = Name::Invalid;
	float m_radius = 0.f;
	float m_durationSeconds = 0.f;
	AbilityScalingComponent	m_scaling;
	AbilityRenderComponent	m_renderComp;
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
	RolledAoEEffectComponent m_aoeEffectOnHit;
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