// Bradley Christensen - 2022-2023
#include "Engine/Core/Time.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



//-----------------------------------------------------------------------------------------------
double InitializeTime(LARGE_INTEGER& out_initialTime)
{
    LARGE_INTEGER countsPerSecond;
    QueryPerformanceFrequency(&countsPerSecond);
    QueryPerformanceCounter(&out_initialTime);
    return(1.0 / static_cast< double >(countsPerSecond.QuadPart));
}



//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime(initialTime);
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;
	return static_cast<double>(elapsedCountsSinceInitialTime) * secondsPerCount;
}



//-----------------------------------------------------------------------------------------------
float GetCurrentTimeSecondsF()
{
	return static_cast<float>(GetCurrentTimeSeconds());
}