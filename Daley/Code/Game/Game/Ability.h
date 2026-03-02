// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Math/IntVec2.h"
#include <vector>



class VertexBuffer;
struct EntityDef;
struct ProjectileHitAbilityDef;
struct Vec2;
struct AbilityDef;



//----------------------------------------------------------------------------------------------------------------------
enum class AbilityTargetingMode
{
	ClosestToGoal,
};



//----------------------------------------------------------------------------------------------------------------------
class Ability
{
public:

	virtual ~Ability() = default;
	virtual void Update(float deltaSeconds, Vec2 const& location) = 0;
	virtual Ability* DeepCopy() const = 0;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const = 0;
	virtual void GetDebugString(std::string& out_string) const = 0;

public:

	AbilityDef const* m_abilityDef		= nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class ProjectileHitAbility : public Ability
{
public:

	explicit ProjectileHitAbility(ProjectileHitAbilityDef const& def);

	virtual void Update(float deltaSeconds, Vec2 const& location) override;
	virtual Ability* DeepCopy() const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void GetDebugString(std::string& out_string) const override;

public:

	Name m_projectileDefName			= Name::Invalid;
	float m_damage						= 1.f;
	float m_attacksPerSecond			= 1.f;
	float m_projSpeedUnitsPerSec		= 1.f;
	float m_minRange					= 0.f;
	float m_maxRange					= 10.f;
	float m_splashDamage				= 0.f;
	float m_splashRadius				= 0.f;
	float m_accumulatedAttackTime		= 0.f;
	AbilityTargetingMode m_targetingMode = AbilityTargetingMode::ClosestToGoal;

	float m_minRangeAtTimeOfCache = -1.f;
	float m_maxRangeAtTimeOfCache = -1.f;
	std::vector<IntVec2> m_pathTilesInRange;
};