// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Time/IndependentClock.h"



//----------------------------------------------------------------------------------------------------------------------
struct CTime
{
    CTime() = default;

	IndependentClock m_clock;
};
