// Bradley Christensen - 2022-2025
#include "IndependentClock.h"
#include "Engine/Core/ErrorUtils.h"
#include <chrono>
#include "Timer.h"



//----------------------------------------------------------------------------------------------------------------------
IndependentClock::IndependentClock()
{
	std::chrono::high_resolution_clock clock;
	m_lastUpdatedTimeNano = clock.now().time_since_epoch().count();
}



//----------------------------------------------------------------------------------------------------------------------
void IndependentClock::Update()
{
	std::chrono::high_resolution_clock clock;

	std::chrono::time_point<std::chrono::high_resolution_clock> now = clock.now();
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdatedTime = std::chrono::high_resolution_clock::time_point{ std::chrono::high_resolution_clock::duration(m_lastUpdatedTimeNano) };
	std::chrono::duration<double> secondsPassedSinceLastUpdated = now - lastUpdatedTime;

	m_deltaSeconds = secondsPassedSinceLastUpdated.count() * m_timeDilation;
	m_currentTimeSeconds += m_deltaSeconds;
	m_lastUpdatedTimeNano = now.time_since_epoch().count();
}



//----------------------------------------------------------------------------------------------------------------------
void IndependentClock::Update(double deltaSeconds)
{
	m_deltaSeconds = deltaSeconds * m_timeDilation;
	m_currentTimeSeconds += m_deltaSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
double IndependentClock::GetCurrentTimeSeconds() const
{
	return m_currentTimeSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
float IndependentClock::GetCurrentTimeSecondsF() const
{
	return static_cast<float>(m_currentTimeSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
double IndependentClock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}



//----------------------------------------------------------------------------------------------------------------------
float IndependentClock::GetDeltaSecondsF() const
{
	return static_cast<float>(m_deltaSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
void IndependentClock::SetLocalTimeDilation(double timeDilation)
{
	m_timeDilation = timeDilation;
}



//----------------------------------------------------------------------------------------------------------------------
double IndependentClock::GetLocalTimeDilation() const
{
	return m_timeDilation;
}



//----------------------------------------------------------------------------------------------------------------------
float IndependentClock::GetLocalTimeDilationF() const
{
	return static_cast<float>(m_timeDilation);
}