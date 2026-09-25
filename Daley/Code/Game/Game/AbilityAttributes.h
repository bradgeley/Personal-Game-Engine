// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <array>



struct EntityDebugContext;



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

	// Modifiers (Add: Additive, Multi: Multiplicative)
    AttackSpeed_Multi,
    Range_Multi,
    AreaOfEffect_Multi,
    AreaDamage_Mulit,
    ProjectileSpeed_Multi,
    CritChance_Add,
    CritMulti_Multi,
    ChainCount_Add,
    ChainChance_Add,
    ChainDistance_Multi,
    MultishotCount_Add,

    Count
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAttributes
{
    AbilityAttributes();
    explicit AbilityAttributes(void const* xmlElement);

	float GetValue(EAbilityAttribute attribute) const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

	void operator+=(AbilityAttributes const& other);
	AbilityAttributes operator+(AbilityAttributes const& other) const;
	AbilityAttributes operator*(float value) const;

	static std::array<Name, (int) EAbilityAttribute::Count> const& GetAttributeNames();
    std::array<float, (int) EAbilityAttribute::Count> m_values;
};