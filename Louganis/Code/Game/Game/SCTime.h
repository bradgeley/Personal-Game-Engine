// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Time/Timer.h"
#include "TimeOfDay.h"



//----------------------------------------------------------------------------------------------------------------------
class SCTime
{
public:

	int			m_dayCount										= 0;
	TimeOfDay	m_timeOfDay										= TimeOfDay::Dawn;
	bool		m_isTransitioning								= false;
	double		m_timeOfDayDurations[(int) TimeOfDay::Count]	= { 15.0, 30.0, 15.0, 30.0 };
	double		m_timeOfDayTransitionDuration					= 10.0; // Transition happens between each state, so the total day cycle duration is transition * N + each state duration

	double		m_entityTimeDilation							= 1.0;
	Timer		m_dayTimer										= Timer();
};