// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <array>



//----------------------------------------------------------------------------------------------------------------------
// All ability attributes default to 0 (for now)
// 
// NEW ATTRIBUTES need to update the following:
// - AbilityAttributes.cpp constructor
// - EAbilityAttribute enum
// - AbilityAttributes::GetAttributeNames
//
enum class EAbilityAttribute
{
    // Base Damage Values
    Physical,
    Burn,
    Poison,
    SlowDuration,
    HasteDuration,

    // Modifiers
    AttackSpeed,
    Range,
    AreaOfEffect,
    AreaDamage,
    ProjectileSpeed,
    CritChance,
    CritMulti,
    ChainCount,
    ChainChance,
    ChainDistance,
    MultishotCount,

    Count
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAttributes
{
    AbilityAttributes();
    explicit AbilityAttributes(void const* xmlElement);

	void operator+=(AbilityAttributes const& other);
	AbilityAttributes operator+(AbilityAttributes const& other) const;
	AbilityAttributes operator*(float value) const;

	static std::array<Name, (int) EAbilityAttribute::Count> const& GetAttributeNames();
    std::array<float, (int) EAbilityAttribute::Count> m_values;
};