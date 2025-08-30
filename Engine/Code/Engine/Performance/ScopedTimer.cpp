// Bradley Christensen - 2022-2025
#include "ScopedTimer.h"
#include "Engine/Time/Time.h"
#include "Engine/Debug/DevConsole.h"
#include "PerformanceDebugWindow.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
ScopedTimer::ScopedTimer(Name name) : m_name(name)
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
		std::string line = StringUtils::StringF("Scoped Timer: %s: %f%s", m_name.ToCStr(), deltaSeconds, "ms");
		g_devConsole->AddLine(line);
	}
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::PerfWindowScopedTimer(Name sectionName, Name rowName, Rgba8 tint /*= Rgba8::White*/) : m_tint(tint)
{
	if (g_performanceDebugWindow)
	{
		m_perfSectionID = g_performanceDebugWindow->GetOrCreateSectionID(sectionName);
		m_perfRowID = g_performanceDebugWindow->GetOrCreateRowID(m_perfSectionID, rowName);
	}
	m_startTimeSeconds = GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::PerfWindowScopedTimer(int sectionID, int rowID, Rgba8 tint /*= Rgba8::White*/) : m_perfSectionID(sectionID), m_perfRowID(rowID), m_tint(tint)
{
	m_startTimeSeconds = GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::~PerfWindowScopedTimer()
{
	PerfItemData item;
	item.m_startTime = m_startTimeSeconds;
	item.m_endTime = GetCurrentTimeSeconds();
	item.m_tint = m_tint;
	if (g_performanceDebugWindow)
	{
		g_performanceDebugWindow->LogItem(item, m_perfSectionID, m_perfRowID);
	}
}
