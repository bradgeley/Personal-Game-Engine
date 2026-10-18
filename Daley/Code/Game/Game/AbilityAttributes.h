// Bradley Christensen - 2022-2026
#pragma once
#include "AttributeDisplayInfo.h"
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
enum class AbilityAttribute : uint8_t
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

    // Enemy Attributes
    PhysicalResistance,
    BurnResistance,
    PoisonResistance,
    SlowResistance,
    Health_Multi,
	MoveSpeed_Multi,

    Count
};



//----------------------------------------------------------------------------------------------------------------------
struct AbilityAttributes
{
public:

    AbilityAttributes() = default;
    explicit AbilityAttributes(void const* xmlElement);

    float GetValue(AbilityAttribute attribute) const;
    void SetValue(AbilityAttribute attribute, float value);
    void AddValue(AbilityAttribute attribute, float delta);

	void AppendDebugString(EntityDebugContext& debugContext) const;

	void operator+=(AbilityAttributes const& other);
	AbilityAttributes operator+(AbilityAttributes const& other) const;
	AbilityAttributes operator*(float value) const;

	static std::array<Name, (int) AbilityAttribute::Count> const& GetAttributeNames();
	static AttributeDisplayInfo const& GetAttributeDisplayInfo(AbilityAttribute attribute);
    static std::string FormatAttribute(AbilityAttribute attr, float value);

public:

    static constexpr int s_maxAttributes = 10;

public:

    uint8_t m_count = 0;
    std::array<AbilityAttribute, s_maxAttributes> m_keys = {};
    std::array<float, s_maxAttributes> m_values = {};
};