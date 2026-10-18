// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Time/IndependentClock.h"
#include <cstdint>
#include <string>



struct HitPayload;



//----------------------------------------------------------------------------------------------------------------------
enum class TimeFlags : uint8_t
{
	SlowedThisFrame = 1 << 0,
	HastedThisFrame = 1 << 1,
};



//----------------------------------------------------------------------------------------------------------------------
struct CTime
{
public:

    CTime() = default;

	bool IsSlowed() const;
	bool IsHasted() const;
	void TakePayload(HitPayload const& payload);

	bool GetIsSlowedThisFrame() const;
	bool GetIsHastedThisFrame() const;

	void SetIsSlowedThisFrame(bool isSlowed);
	void SetIsHastedThisFrame(bool isHasted);

	void AppendDebugString(std::string& out_string) const;

public:

	IndependentClock m_clock;

	TimeFlags m_timeFlags = static_cast<TimeFlags>(0);
	float m_remainingSlowDuration = 0.f;
	float m_remainingHasteDuration = 0.f;
	float m_slowResistance = 0.f;
};
