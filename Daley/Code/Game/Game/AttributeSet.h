// Bradley Christensen - 2022-2026
#pragma once
#include "AttributeDisplayInfo.h"
#include "Engine/Core/Name.h"
#include <array>
#include <cstdint>



struct EntityDebugContext;



//----------------------------------------------------------------------------------------------------------------------
struct AttributeSet
{
public:

    AttributeSet();
    explicit AttributeSet(void const* xmlElement);

    float GetValue(Name attribute, float defaultValue = 0.f, bool errorIfNotFound = false) const;
    void SetValue(Name attribute, float value);
    void AddValue(Name attribute, float delta);
    void Clear();

	void AppendDebugString(EntityDebugContext& debugContext) const;

	void operator+=(AttributeSet const& other);
	AttributeSet operator+(AttributeSet const& other) const;
	AttributeSet operator*(float value) const;

    static std::string FormatAttribute(Name attr, float value);

public:

    static constexpr int s_maxAttributes = 32;

public:

    uint8_t m_count = 0;
    std::array<Name, s_maxAttributes> m_keys = {};
    std::array<float, s_maxAttributes> m_values = {};
};