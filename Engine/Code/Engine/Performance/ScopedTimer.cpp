// Bradley Christensen - 2022-2026
#include "ScopedTimer.h"
#include "Engine/Time/Time.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "PerformanceDebugWindow.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
ScopedTimer::ScopedTimer(Name name) : m_name(name)
{
	m_startTimeSeconds = Time::GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
ScopedTimer::~ScopedTimer()
{
	double endTimeSeconds = Time::GetCurrentTimeSeconds();
	double deltaSeconds = endTimeSeconds - m_startTimeSeconds;
	deltaSeconds *= 1000.0;

	DevConsoleUtils::Log(Rgba8::Orchid, "Scoped Timer: %s: %f%s", m_name.ToCStr(), deltaSeconds, "ms");
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::PerfWindowScopedTimer(Name sectionName, Name rowName, Rgba8 tint /*= Rgba8::White*/) : m_tint(tint)
{
	if (g_performanceDebugWindow)
	{
		m_perfSectionID = g_performanceDebugWindow->GetOrCreateSectionID(sectionName);
		m_perfRowID = g_performanceDebugWindow->GetOrCreateRowID(m_perfSectionID, rowName);
	}
	m_startTimeSeconds = Time::GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::PerfWindowScopedTimer(int sectionID, int rowID, Rgba8 tint /*= Rgba8::White*/) : m_perfSectionID(sectionID), m_perfRowID(rowID), m_tint(tint)
{
	m_startTimeSeconds = Time::GetCurrentTimeSeconds();
}



//----------------------------------------------------------------------------------------------------------------------
PerfWindowScopedTimer::~PerfWindowScopedTimer()
{
	PerfItemData item;
	item.m_startTime = m_startTimeSeconds;
	item.m_endTime = Time::GetCurrentTimeSeconds();
	item.m_tint = m_tint;
	if (g_performanceDebugWindow)
	{
		g_performanceDebugWindow->LogItem(item, m_perfSectionID, m_perfRowID);
	}
}
