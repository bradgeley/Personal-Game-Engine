// Bradley Christensen - 2022-2026
#include "Engine/Time/Time.h"
#include "Engine/Core/StringUtils.h"
#include <chrono>



//-----------------------------------------------------------------------------------------------
double Time::GetCurrentTimeSeconds()
{
	static std::chrono::high_resolution_clock clock;
	static std::chrono::time_point initialTime = clock.now();
	std::chrono::time_point now = clock.now();
	std::chrono::duration<double> timePassed = now - initialTime;
	return timePassed.count();
}



//-----------------------------------------------------------------------------------------------
float Time::GetCurrentTimeSecondsF()
{
	return static_cast<float>(GetCurrentTimeSeconds());
}



//-----------------------------------------------------------------------------------------------
std::string Time::GetDisplayString(double seconds)
{
	if (seconds >= 3600 * 24 * 365)
	{
		return StringUtils::StringF("%.2f year", seconds / (3600 * 24 * 365));
	}
	if (seconds >= 3600 * 24)
	{
		return StringUtils::StringF("%.2f day", seconds / (3600 * 24));
	}
	if (seconds >= 3600)
	{
		return StringUtils::StringF("%.2f hr", seconds / 3600);
	}
	if (seconds >= 60)
	{
		return StringUtils::StringF("%.2f min", seconds / 60);
	}
	if (seconds >= 0.99)
	{
		return StringUtils::StringF("%.2f sec", seconds);
	}
	if (seconds >= 0.001)
	{
		return StringUtils::StringF("%.f ms", seconds * 1000);
	}
	if (seconds >= 0.000'001)
	{
		return StringUtils::StringF("%.f us", seconds * 1000000);
	}
	if (seconds >= 0.000'000'000)
	{
		return StringUtils::StringF("%.f ns", seconds * 1000000000);
	}

	return StringUtils::StringF("%.2f sec", seconds);
}
