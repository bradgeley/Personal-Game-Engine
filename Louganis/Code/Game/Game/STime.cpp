// Bradley Christensen - 2022-2025
#include "STime.h"
#include "SCTime.h"
#include "SCWorld.h"
#include "CTime.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void STime::Startup()
{
	AddReadDependencies<SCWorld>();
	AddWriteDependencies<SCTime, CTime>(); // SCTime updates any CTime's that are children of SCTime's world clock

	SCTime& scTime = g_ecs->GetSingleton<SCTime>();
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scTime.m_dayCount = 0;
	scTime.m_isTransitioning = false;
	scTime.m_timeOfDay = TimeOfDay::Dawn;
	scTime.m_dayTimer.Set(nullptr, scWorld.m_worldSettings.m_timeOfDayDurations[(int) scTime.m_timeOfDay]);
}



//----------------------------------------------------------------------------------------------------------------------
void STime::Run(SystemContext const& context)
{
	auto& timeStorage = g_ecs->GetArrayStorage<CTime>();
	SCTime& scTime = g_ecs->GetSingleton<SCTime>();
	SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();

	// Time of day
	bool completedState = scTime.m_dayTimer.Update(context.m_deltaSeconds);

	if (completedState)
	{
		if (scTime.m_isTransitioning)
		{
			scTime.m_timeOfDay = (TimeOfDay) MathUtils::IncrementIntInRange((int) scTime.m_timeOfDay, 0, (int) TimeOfDay::Count - 1, true);
			scTime.m_dayTimer.Set(nullptr, scWorld.m_worldSettings.m_timeOfDayDurations[(int) scTime.m_timeOfDay]);
			scTime.m_isTransitioning = false;
		}
		else
		{
			scTime.m_isTransitioning = true;
			scTime.m_dayTimer.Set(nullptr, scWorld.m_worldSettings.m_timeOfDayTransitionDuration);
		}
	}

	// Entity clocks
	for (auto it = g_ecs->Iterate<CTime>(context); it.IsValid(); ++it)
	{
		CTime& cTime = timeStorage[it];

		cTime.m_clock.Update(context.m_deltaSeconds * scTime.m_entityTimeDilation);
	}
}