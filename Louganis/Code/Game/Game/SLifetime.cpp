// Bradley Christensen - 2022-2025
#include "SLifetime.h"
#include "CLifetime.h"
#include "CTime.h"
#include "SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SLifetime::Startup()
{
	AddReadDependencies<CTime>();
	AddWriteDependencies<CLifetime, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SLifetime::Run(SystemContext const& context)
{
	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();
	auto& lifetimeStorage = g_ecs->GetArrayStorage<CLifetime>();

	for (auto it = g_ecs->Iterate<CLifetime>(context); it.IsValid(); ++it)
	{
		CLifetime& lifetime = lifetimeStorage[it];
		if (lifetime.m_lifetimeRemaining >= 0.f)
		{
			CTime* time = g_ecs->GetComponent<CTime>(it);
			float timeDilation = 1.f;
			if (time)
			{
				timeDilation = time->m_clock.GetTimeDilationF();
			}

			lifetime.m_lifetimeRemaining -= context.m_deltaSeconds * timeDilation;

			if (lifetime.m_lifetimeRemaining < 0.f)
			{
				entityFactory.m_entitiesToDestroy.push_back(it.m_currentIndex);
			}
		}
	}
}

 

//----------------------------------------------------------------------------------------------------------------------
void SLifetime::Shutdown()
{

}
