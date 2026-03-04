// Bradley Christensen - 2022-2026
#pragma once
#include "Ability.h"
#include <vector>



struct AbilityDef;



//----------------------------------------------------------------------------------------------------------------------
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

	std::vector<Ability*> m_abilities;
};
