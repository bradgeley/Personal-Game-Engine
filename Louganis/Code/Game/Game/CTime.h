// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Time/IndependentClock.h"



//----------------------------------------------------------------------------------------------------------------------
struct CTime
{
    CTime() = default;

	IndependentClock m_clock;
};
