// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <vector>



class Weapon;



//----------------------------------------------------------------------------------------------------------------------
struct WeaponDef
{
public:

	virtual ~WeaponDef() = default;
	virtual Weapon* MakeBaseWeapon() const = 0;

public:

	static void LoadFromXML();
	static void Shutdown();
	static WeaponDef const* GetWeaponDef(uint8_t id);
	static WeaponDef const* GetWeaponDef(Name name);
	static int GetWeaponDefID(Name name);

private:

	static std::vector<WeaponDef*> s_weaponDefs;

public:

	Name m_name = Name::Invalid;
};



//----------------------------------------------------------------------------------------------------------------------
struct ProjectileHitWeaponDef : public WeaponDef
{
public:

	explicit ProjectileHitWeaponDef(void const* xmlElement);

	virtual Weapon* MakeBaseWeapon() const override;

public:

	Name m_projectileDefName			= Name::Invalid;
	float m_baseDamage					= 1.f;
	float m_baseAttacksPerSecond		= 1.f;
	float m_baseProjSpeedUnitsPerSec	= 1.f;
	float m_baseMinRange				= 0.f;
	float m_baseMaxRange				= 1.f;
};