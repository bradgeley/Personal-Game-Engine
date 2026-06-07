// Bradley Christensen - 2022-2026
#pragma once
#include <string>



//----------------------------------------------------------------------------------------------------------------------
// Time
//
// Common time related functions
//
namespace Time
{
	double GetCurrentTimeSeconds();
	float GetCurrentTimeSecondsF();
	std::string GetDisplayString(double seconds);
}