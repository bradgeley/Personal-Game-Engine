// Bradley Christensen - 2024
#pragma once
#include <string>



//----------------------------------------------------------------------------------------------------------------------
class ScopedTimer
{
public:

	ScopedTimer(std::string const& name);
	~ScopedTimer();

	double m_startTimeSeconds	= 0;
	std::string m_name			= nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class PerfWindowScopedTimer
{
public:

	PerfWindowScopedTimer(std::string const& sectionName, std::string const& rowName);
	PerfWindowScopedTimer(int sectionID, int rowID);
	~PerfWindowScopedTimer();

	double m_startTimeSeconds	= 0;
	int m_perfSectionID			= -1;
	int m_perfRowID				= -1;
};