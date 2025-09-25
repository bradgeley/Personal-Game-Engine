// Bradley Christensen - 2022-2025
#include "Timer.h"
#include "Clock.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"



//-----------------------------------------------------------------------------------------------------------------------
Timer::Timer(Clock* parentClock /*= nullptr*/, double durationSeconds /*= 0.0*/, bool looping /*= false*/) 
	: m_parentClock(parentClock), m_durationSeconds(durationSeconds), m_remainingSeconds(durationSeconds), m_looping(looping)
{
}



//-----------------------------------------------------------------------------------------------------------------------
Timer::Timer(double durationSeconds, bool looping)
	: m_parentClock(nullptr), m_durationSeconds(durationSeconds), m_remainingSeconds(durationSeconds), m_looping(looping)
{
}



//-----------------------------------------------------------------------------------------------------------------------
bool Timer::Set(Clock* parentClock, double durationSeconds, bool looping /*= false*/)
{
	ASSERT_OR_DIE(durationSeconds > 0.0, "Timer being set with 0 or negative duration.");

	if (durationSeconds <= 0.f)
	{
		return false;
	}

	m_looping = looping;
	m_parentClock = parentClock;
	m_durationSeconds = durationSeconds;
	m_remainingSeconds = m_durationSeconds;

	return true;
}



//-----------------------------------------------------------------------------------------------------------------------
bool Timer::Update() 
{
	ASSERT_OR_DIE(m_parentClock != nullptr, "Cannot update timer with a null parent clock.");

	if (m_parentClock == nullptr || m_durationSeconds <= 0.0 || m_remainingSeconds <= 0.0)
	{
		return false;
	}

	double deltaSeconds = m_parentClock->GetDeltaSeconds();
	return Update(deltaSeconds);
}



//-----------------------------------------------------------------------------------------------------------------------
bool Timer::Update(double deltaSeconds)
{
	m_remainingSeconds -= deltaSeconds;

	if (m_remainingSeconds <= 0.0)
	{
		if (m_looping)
		{
			m_remainingSeconds += m_durationSeconds;
			if (m_remainingSeconds <= 0.0)
			{
				// We += for more accuracy in 99% of cases, but if we overshot in a single frame (really slow frames or low duration timer), just reset to duration
				m_remainingSeconds = m_durationSeconds;
			}
		}
		else
		{
			m_remainingSeconds = 0.0;
		}
		return true;
	}

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
Clock* Timer::GetParentClock() const
{
	return m_parentClock;
}



//----------------------------------------------------------------------------------------------------------------------
double Timer::GetDurationSeconds() const
{
	return m_durationSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
double Timer::GetRemainingSeconds() const
{
	return m_remainingSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
double Timer::GetRemainingFraction() const
{
	if (m_durationSeconds <= 0.0)
	{
		return 1.0;
	}
	double remainingFraction = m_remainingSeconds / m_durationSeconds;
	return MathUtils::Clamp01(remainingFraction);
}



//----------------------------------------------------------------------------------------------------------------------
double Timer::GetElapsedFraction() const
{
	if (m_durationSeconds <= 0.0)
	{
		return 0.0;
	}
	double elapsedFraction = 1.0 - (m_remainingSeconds / m_durationSeconds);
	return MathUtils::Clamp01(elapsedFraction);
}




//----------------------------------------------------------------------------------------------------------------------
bool Timer::IsComplete() const
{
	return m_remainingSeconds <= 0.0;
}