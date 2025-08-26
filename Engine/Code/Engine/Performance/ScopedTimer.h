// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Core/Name.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
class ScopedTimer
{
public:

	ScopedTimer(Name name);
	~ScopedTimer();

public:

	double m_startTimeSeconds	= 0;
	Name m_name;
};



//----------------------------------------------------------------------------------------------------------------------
class PerfWindowScopedTimer
{
public:

	PerfWindowScopedTimer(Name sectionName, Name rowName, Rgba8 tint = Rgba8::White);
	PerfWindowScopedTimer(int sectionID, int rowID, Rgba8 tint = Rgba8::White);
	~PerfWindowScopedTimer();

public:

	double m_startTimeSeconds	= 0;
	int m_perfSectionID			= -1;
	int m_perfRowID				= -1;
	Rgba8 m_tint				= Rgba8::White;
};