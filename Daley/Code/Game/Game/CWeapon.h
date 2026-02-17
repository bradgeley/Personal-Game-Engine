// Bradley Christensen - 2022-2025
#pragma once
#include "Weapon.h"
#include <vector>



struct WeaponDef;



//----------------------------------------------------------------------------------------------------------------------
struct CWeapon
{
public:

    explicit CWeapon() = default;
    explicit CWeapon(void const* xmlElement);
    explicit CWeapon(CWeapon const& copyFrom);
    CWeapon(CWeapon&& moveFrom) noexcept;
    ~CWeapon();

    CWeapon& operator=(CWeapon const& copyFrom);
    CWeapon& operator=(CWeapon&& moveFrom) noexcept;
    
public:

	std::vector<Weapon*> m_weapons;
};
