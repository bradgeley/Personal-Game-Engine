// Bradley Christensen - 2024
#pragma once
#include <string>



//----------------------------------------------------------------------------------------------------------------------
class ScopedTimer
{
public:

	ScopedTimer(std::string const& name);
	~ScopedTimer();

	double m_startTimeSeconds = 0;
	std::string m_name = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class ScopedPerfWindowTimer
{
	ScopedPerfWindowTimer(int perfSectionId);
	~ScopedPerfWindowTimer();

	double m_startTimeSeconds = 0;
	int m_perfSectionId = -1;
};