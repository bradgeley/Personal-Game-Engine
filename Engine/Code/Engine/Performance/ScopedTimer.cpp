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
		std::string line = StringUtils::StringF("Scoped Timer: %s: %f%s", m_name.c_str(), deltaSeconds, "ms");
		g_devConsole->AddLine(line);
	}
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::PerfWindowScopedTimer(std::string const& sectionName, std::string const& rowName)
{
	if (g_performanceDebugWindow)
	{
		m_perfSectionID = g_performanceDebugWindow->GetOrCreateSectionID(sectionName);
		m_perfRowID = g_performanceDebugWindow->GetOrCreateRowID(m_perfSectionID, rowName);
	}
	m_startTimeSeconds = GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::PerfWindowScopedTimer(int sectionID, int rowID) : m_perfSectionID(sectionID), m_perfRowID(rowID)
{
	m_startTimeSeconds = GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::~PerfWindowScopedTimer()
{
	PerfItemData item;
	item.m_startTime = m_startTimeSeconds;
	item.m_endTime = GetCurrentTimeSeconds();
	if (g_performanceDebugWindow)
	{
		g_performanceDebugWindow->LogItem(item, m_perfSectionID, m_perfRowID);
	}
}
