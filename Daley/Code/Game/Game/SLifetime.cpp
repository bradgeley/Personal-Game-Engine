// Bradley Christensen - 2022-2026
#include "SLifetime.h"
#include "CLifetime.h"
#include "CTime.h"
#include "SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SLifetime::Startup()
{
	AddReadDependencies<CTime>();
	AddWriteDependencies< CLifetime, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SLifetime::Run(SystemContext const& context)
{
	SCEntityFactory& entityFactory = g_ecs->GetSingleton<SCEntityFactory>();
	auto& lifetimeStorage = g_ecs->GetArrayStorage<CLifetime>();
	auto& timeStorage = g_ecs->GetArrayStorage<CTime>();

	BitMask timeBitMask = g_ecs->GetComponentBitMask<CTime>();

	for (auto it = g_ecs->Iterate<CLifetime>(context); it.IsValid(); ++it)
	{
		CLifetime& lifetime = lifetimeStorage[it];
		if (lifetime.m_lifetimeRemaining >= 0.f)
		{
			float timeDilation = 1.f;
			if (g_ecs->DoesEntityHaveComponentsUnsafe(it.m_currentIndex, timeBitMask))
			{
				CTime const& time = timeStorage[it];
				timeDilation = time.m_clock.GetTimeDilationF();
			}

			lifetime.m_lifetimeRemaining -= context.m_deltaSeconds * timeDilation;

			if (lifetime.m_lifetimeRemaining < 0.f)
			{
				EntityID id = it.GetEntityID();
				entityFactory.m_entitiesToDestroy.push_back(id);
			}
		}
	}
}

 

//----------------------------------------------------------------------------------------------------------------------
void SLifetime::Shutdown()
{

}
