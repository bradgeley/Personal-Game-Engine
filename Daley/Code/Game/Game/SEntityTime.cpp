// Bradley Christensen - 2022-2026
#include "SEntityTime.h"
#include "SCTime.h"
#include "CTime.h"
#include "GameCommon.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SEntityTime::Startup()
{
	AddReadDependencies<SCTime>();
	AddWriteDependencies<CTime>();

	m_runWhilePaused = false;

	int numThreads = std::thread::hardware_concurrency();
	if (numThreads > 1)
	{
		SetSystemSplittingNumJobs(numThreads - 1);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SEntityTime::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCTime const& scTime = context.GetSingletonConst<SCTime>();

	// Write Dependencies
	auto& timeStorage = context.GetArrayStorage<CTime>();

	for (auto it = context.Iterate<CTime>(); it.IsValid(); ++it)
	{
		CTime& cTime = timeStorage[it];

		cTime.m_remainingSlowDuration = std::max(0.f, cTime.m_remainingSlowDuration - context.m_deltaSeconds);

		if (cTime.m_remainingSlowDuration > 0.f)
		{
			cTime.m_clock.SetTimeDilation(StaticGameSettings::s_slowStatusTimeDilation);
		}
		else
		{
			cTime.m_clock.SetTimeDilation(1.0);
		}

		cTime.m_clock.Update(context.m_deltaSeconds * scTime.m_entityTimeDilation);
	}
}