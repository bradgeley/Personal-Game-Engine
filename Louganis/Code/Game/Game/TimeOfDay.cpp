// Bradley Christensen - 2022-2025
#include "TimeOfDay.h"
#include "Engine/Math/MathUtils.h"
#include "SCTime.h"



//----------------------------------------------------------------------------------------------------------------------
Rgba8 TimeOfDayUtils::GetTimeOfDayTint(TimeOfDay const& timeOfDay)
{
	switch (timeOfDay)
	{
		case TimeOfDay::Dawn:   return Rgba8(255, 200, 124, 255);
		case TimeOfDay::Day:    return Rgba8(255, 255, 255, 255);
		case TimeOfDay::Dusk:   return Rgba8(200, 100, 100, 255);
		case TimeOfDay::Night:  return Rgba8(50,   50,  50, 255);
		default:            
			return Rgba8::White;
	}
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 TimeOfDayUtils::GetTimeOfDayTransitionTint(TimeOfDay const& timeOfDay, float timeOfDayTransitionPercent)
{
	Rgba8 tintA = GetTimeOfDayTint(timeOfDay);
	TimeOfDay nextTimeOfDay = (TimeOfDay) MathUtils::IncrementIntInRange((int) timeOfDay, 0, (int) TimeOfDay::Count - 1, true);
	Rgba8 tintB = GetTimeOfDayTint(nextTimeOfDay);
	return Rgba8::Lerp(tintA, tintB, timeOfDayTransitionPercent);
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 TimeOfDayUtils::GetTimeOfDayTint(SCTime const& scTime)
{
	return GetTimeOfDayTransitionTint(scTime.m_timeOfDay, scTime.m_isTransitioning ? (float) scTime.m_dayTimer.GetElapsedFraction() : 0.f);
}
