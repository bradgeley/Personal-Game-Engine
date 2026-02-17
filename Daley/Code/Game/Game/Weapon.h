// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"



struct EntityDef;
struct ProjectileHitWeaponDef;
struct WeaponDef;



//----------------------------------------------------------------------------------------------------------------------
class Weapon
{
public:

	virtual ~Weapon() = default;
	virtual void Update(float deltaSeconds) = 0;
	virtual Weapon* DeepCopy() const = 0;

public:

	WeaponDef const* m_weaponDef		= nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class ProjectileHitWeapon : public Weapon
{
public:

	explicit ProjectileHitWeapon(ProjectileHitWeaponDef const& def);

	virtual void Update(float deltaSeconds) override;
	virtual Weapon* DeepCopy() const override;

public:

	EntityDef const* m_projectileDef	= nullptr;
	float m_damage						= 1.f;
	float m_attacksPerSecond			= 1.f;
	float m_projSpeedUnitsPerSec		= 1.f;
};