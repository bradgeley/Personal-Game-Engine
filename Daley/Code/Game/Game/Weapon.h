// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Math/IntVec2.h"
#include <vector>



class VertexBuffer;
struct EntityDef;
struct ProjectileHitWeaponDef;
struct Vec2;
struct WeaponDef;



//----------------------------------------------------------------------------------------------------------------------
enum class WeaponTargetingMode
{
	ClosestToGoal,
};



//----------------------------------------------------------------------------------------------------------------------
class Weapon
{
public:

	virtual ~Weapon() = default;
	virtual void Update(float deltaSeconds, Vec2 const& location) = 0;
	virtual Weapon* DeepCopy() const = 0;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const = 0;
	virtual void GetDebugString(std::string& out_string) const = 0;

public:

	WeaponDef const* m_weaponDef		= nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class ProjectileHitWeapon : public Weapon
{
public:

	explicit ProjectileHitWeapon(ProjectileHitWeaponDef const& def);

	virtual void Update(float deltaSeconds, Vec2 const& location) override;
	virtual Weapon* DeepCopy() const override;
	virtual void AddDebugVerts(VertexBuffer& out_vbo, Vec2 const& location) const override;
	virtual void GetDebugString(std::string& out_string) const override;

public:

	Name m_projectileDefName			= Name::Invalid;
	float m_damage						= 1.f;
	float m_attacksPerSecond			= 1.f;
	float m_projSpeedUnitsPerSec		= 1.f;
	float m_minRange					= 0.f;
	float m_maxRange					= 10.f;
	float m_accumulatedAttackTime		= 0.f;
	WeaponTargetingMode m_targetingMode = WeaponTargetingMode::ClosestToGoal;

	float m_minRangeAtTimeOfCache = -1.f;
	float m_maxRangeAtTimeOfCache = -1.f;
	std::vector<IntVec2> m_pathTilesInRange;
};