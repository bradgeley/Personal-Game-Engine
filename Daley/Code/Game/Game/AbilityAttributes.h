// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <array>
#include <cstdint>



struct EntityDebugContext;



//----------------------------------------------------------------------------------------------------------------------
// All ability attributes default to 0 (for now)
// 
// NEW ATTRIBUTES need to update the following:
// - AbilityAttributes.cpp constructor
// - EAbilityAttribute enum
// - AbilityAttributes::GetAttributeNames
//
enum class AbilityAttribute : uint32_t
{
    // Base Damage Values
    Physical,
    Burn,
    Poison,
    Vulnerability,
    SlowDuration,
    HasteDuration,

    Physical_Multi,
    Burn_Multi,
    Poison_Multi,
    Vulnerability_Multi,
    Slow_Multi,
    Haste_Multi,

	// Modifiers (Add: Additive, Multi: Multiplicative)
    AttackSpeed_Multi,
    Range_Multi,
    AreaOfEffect_Multi,
    AreaDamage_Multi,
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
enum class AttributeDisplayType : uint8_t
{
    FlatValue,          // "15 Damage"
    FlatSigned,         // "+2 Chain Count"
    Percent,            // "25%"
    PercentSigned,      // "+25%"
    MultiplierPercent,  // 1.25 -> "+25%"
    Seconds,            // "2.5s Slow"
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAttributeDisplayInfo
{
    Name m_displayName;
    AttributeDisplayType m_displayType;
    std::string m_format = "";
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAttributes
{
    AbilityAttributes();
    explicit AbilityAttributes(void const* xmlElement);

	float GetValue(AbilityAttribute attribute) const;

	void AppendDebugString(EntityDebugContext& debugContext) const;

	void operator+=(AbilityAttributes const& other);
	AbilityAttributes operator+(AbilityAttributes const& other) const;
	AbilityAttributes operator*(float value) const;

	static std::array<Name, (int) AbilityAttribute::Count> const& GetAttributeNames();
	static AbilityAttributeDisplayInfo const& GetAttributeDisplayInfo(AbilityAttribute attribute);
    static std::string FormatAttribute(AbilityAttribute attr, float value);

    std::array<float, (int) AbilityAttribute::Count> m_values;
};