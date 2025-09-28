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

	double		m_entityTimeDilation							= 1.0;
	Timer		m_dayTimer										= Timer();
};