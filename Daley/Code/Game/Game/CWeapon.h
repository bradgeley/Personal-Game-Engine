// Bradley Christensen - 2022-2025
#pragma once
#include "Weapon.h"
#include <vector>



struct WeaponDef;



//----------------------------------------------------------------------------------------------------------------------
struct CWeapon
{
public:

    CWeapon() = default;
    ~CWeapon();
    CWeapon(void const* xmlElement);
    
public:

	std::vector<Weapon*> m_weapons;
};
