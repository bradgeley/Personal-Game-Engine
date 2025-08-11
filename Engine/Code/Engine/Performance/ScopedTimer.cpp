// Bradley Christensen - 2024
#include "ScopedTimer.h"
#include "Engine/Core/Time.h"
#include "Engine/Debug/DevConsole.h"
#include "PerformanceDebugWindow.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
ScopedTimer::ScopedTimer(std::string const& name) : m_name(name)
{
	m_startTimeSeconds = GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
ScopedTimer::~ScopedTimer()
{
	double endTimeSeconds = GetCurrentTimeSeconds();
	double deltaSeconds = endTimeSeconds - m_startTimeSeconds;
	deltaSeconds *= 1000.0;
	if (g_devConsole)
	{
		std::string units = "ms";
		std::string line = StringF("Scoped Timer: %s: %f%s", m_name.c_str(), deltaSeconds, units.c_str());
		g_devConsole->AddLine(line);
	}
}



//----------------------------------------------------------------------------------------------------------------------
ScopedPerfWindowTimer::ScopedPerfWindowTimer(int perfSectionId) : m_perfSectionId(perfSectionId)
{
	m_startTimeSeconds = GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
ScopedPerfWindowTimer::~ScopedPerfWindowTimer()
{
	double endTimeSeconds = GetCurrentTimeSeconds();
	double deltaSeconds = endTimeSeconds - m_startTimeSeconds;
	deltaSeconds *= 1000.0;
	if (g_performancewin)
}
