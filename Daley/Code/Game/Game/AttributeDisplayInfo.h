// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <cstdint>
#include <string>



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
struct AttributeDisplayInfo
{
    Name m_displayName;
    AttributeDisplayType m_displayType;
    std::string m_format = "";
};