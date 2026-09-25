// Bradley Christensen - 2022-2026
#pragma once
#include "AbilityAttributes.h"
#include "AbilityFlags.h"
#include <vector>



class Ability;
struct AbilityDef;
struct AbilityBuff;
struct EntityDebugContext;



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

	void AppendDebugString(EntityDebugContext& debugContext) const;

    CAbility& operator=(CAbility const& copyFrom);
    CAbility& operator=(CAbility&& moveFrom) noexcept;
    
public:
    
	bool m_needsAttributeRebuild = true;

    AbilityAttributes m_attributes;
    AbilityFlags m_abilityFlags;
	std::vector<AbilityBuff*> m_activeBuffs; // todo
	std::vector<Ability*> m_abilities;
};
