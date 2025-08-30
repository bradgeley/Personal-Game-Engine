// Bradley Christensen - 2022-2025
#pragma once



class Clock;



//-----------------------------------------------------------------------------------------------------------------------
// Timer
//
// Usage: 
// 
// Stored timer location:
// Timer m_timer;
// 
// Initialization:
// m_timer.Set(g_clock, 1.0);
// 
// Updating:
// if (timer.Update()) 
// { 
//	   timer complete logic 
// }
// 
// Update will only return true once for each time the timer is completed.
//
class Timer
{
public:

	explicit Timer(Clock* parentClock = nullptr, double durationSeconds = 0.0, bool looping = false);

	// Mutators
	bool Set(Clock* parentClock, double durationSeconds, bool looping = false);
	bool Update(); // returns whether or not the timer completed this frame

	// Getters
	Clock* GetParentClock() const;
	double GetDurationSeconds() const;
	double GetRemainingSeconds() const;
	double GetRemainingFraction() const;
	double GetElapsedFraction() const;
	bool IsComplete() const;

public:

	Clock* m_parentClock = nullptr;
	double m_durationSeconds = 0.f;
	double m_remainingSeconds = 0.f;
	bool m_looping = false;
};

