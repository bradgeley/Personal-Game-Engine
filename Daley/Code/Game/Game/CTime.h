// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Time/IndependentClock.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
struct CTime
{
public:

    CTime() = default;

	bool IsSlowed() const;

	void AppendDebugString(std::string& out_string) const;

public:

	IndependentClock m_clock;

	float m_remainingSlowDuration = 0.f;
};
