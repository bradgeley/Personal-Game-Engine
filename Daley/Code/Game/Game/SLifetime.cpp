// Bradley Christensen - 2022-2026
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
void SLifetime::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto & timeStorage = context.GetArrayStorageConst<CTime>();

	// Write Dependencies
	SCEntityFactory& entityFactory = context.GetSingleton<SCEntityFactory>();
	auto& lifetimeStorage = context.GetArrayStorage<CLifetime>();

	BitMask timeBitMask = context.GetComponentBitMask<CTime>();

	for (auto it = context.Iterate<CLifetime>(); it.IsValid(); ++it)
	{
		CLifetime& lifetime = lifetimeStorage[it];
		if (lifetime.m_lifetimeRemaining >= 0.f)
		{
			float timeDilation = 1.f;
			if (context.DoesEntityHaveComponentsUnsafe(it.m_currentIndex, timeBitMask))
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
