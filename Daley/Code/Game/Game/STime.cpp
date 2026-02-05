// Bradley Christensen - 2022-2025
#include "STime.h"
#include "SCTime.h"
#include "CTime.h"



//----------------------------------------------------------------------------------------------------------------------
void STime::Startup()
{
	AddWriteDependencies<SCTime, CTime>(); // SCTime updates any CTime's that are children of SCTime's world clock
}



//----------------------------------------------------------------------------------------------------------------------
void STime::Run(SystemContext const& context)
{
	SCTime const& scTime = g_ecs->GetSingleton<SCTime>();
	auto& timeStorage = g_ecs->GetArrayStorage<CTime>();

	for (auto it = g_ecs->Iterate<CTime>(context); it.IsValid(); ++it)
	{
		CTime& cTime = timeStorage[it];

		cTime.m_clock.Update(context.m_deltaSeconds * scTime.m_entityTimeDilation);
	}
}