// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityAttributes.h"
#include <vector>



class Ability;
struct AbilityDef;
struct AbilityBuff;



//----------------------------------------------------------------------------------------------------------------------
// Holds ability-related attributes, buffs, and abilities for one entity
//
struct CAbility
{
public:

    explicit CAbility() = default;
    explicit CAbility(void const* xmlElement);
    explicit CAbility(CAbility const& copyFrom);
    CAbility(CAbility&& moveFrom) noexcept;
    ~CAbility();

    CAbility& operator=(CAbility const& copyFrom);
    CAbility& operator=(CAbility&& moveFrom) noexcept;
    
public:
    
    AbilityAttributes m_attributes;
	std::vector<AbilityBuff*> m_activeBuffs;
	std::vector<Ability*> m_abilities;
};
