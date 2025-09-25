// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/Rgba8.h"



class SCTime;



//----------------------------------------------------------------------------------------------------------------------
enum class TimeOfDay
{
	Dawn = 0,
	Day,
	Dusk,
	Night,
	Count,
};



namespace TimeOfDayUtils
{
	Rgba8 GetTimeOfDayTint(TimeOfDay const& timeOfDay);
	Rgba8 GetTimeOfDayTransitionTint(TimeOfDay const& timeOfDay, float timeOfDayTransitionPercent);
	Rgba8 GetTimeOfDayTint(SCTime const& scTime);
}