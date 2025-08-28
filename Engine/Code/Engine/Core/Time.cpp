// Bradley Christensen - 2022-2025
#include "Engine/Core/Time.h"
#include <chrono>



//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static std::chrono::high_resolution_clock clock;
	static std::chrono::time_point initialTime = clock.now();
	std::chrono::time_point now = clock.now();
	std::chrono::duration<double> timePassed = now - initialTime;
	return timePassed.count();
}



//-----------------------------------------------------------------------------------------------
float GetCurrentTimeSecondsF()
{
	return static_cast<float>(GetCurrentTimeSeconds());
}