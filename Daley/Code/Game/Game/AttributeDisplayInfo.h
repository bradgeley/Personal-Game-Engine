// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class AttributeDisplayType : uint8_t
{
    Integer,            // "15 Damage"
    IntegerSigned,      // "+15 Damage"
    Decimal,            // "15.5 Damage"
    DecimalSigned,      // "+15.5 Damage"
    Percent,            // "25%"
    PercentSigned,      // "+25%"
    Seconds,            // "2.5s Slow"
};



//----------------------------------------------------------------------------------------------------------------------
struct AttributeDisplayInfo
{
    Name m_displayName;
    AttributeDisplayType m_displayType;
};