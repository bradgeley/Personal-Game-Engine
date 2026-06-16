// Bradley Christensen - 2022-2026
#include "SWorldTime.h"
#include "SCTime.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorldTime::Startup()
{
	AddWriteDependencies<SCTime>(); // SCTime updates any CTime's that are children of SCTime's world clock
}



//----------------------------------------------------------------------------------------------------------------------
void SWorldTime::Run(SystemContext const& context) const
{
	// Write Dependencies
	SCTime& scTime = context.GetSingleton<SCTime>();

	scTime.m_currentTimeSeconds += context.m_deltaSeconds;
}