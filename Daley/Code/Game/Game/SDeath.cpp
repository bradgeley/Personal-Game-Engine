// Bradley Christensen - 2022-2026
#include "SDeath.h"
#include "CAnimation.h"
#include "CDeath.h"
#include "CHealth.h"
#include "CLifetime.h"
#include "SCEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SDeath::Startup()
{
	AddReadDependencies<CHealth>();
	AddWriteDependencies<CAnimation, CDeath, CLifetime>();

	int numThreads = (int) std::thread::hardware_concurrency();
	if (numThreads > 1)
	{
		m_systemSplittingNumJobs = numThreads - 1; // Leave one thread for the main thread to run other systems on
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SDeath::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();

	// Write Dependencies
	auto& animStorage = context.GetArrayStorage<CAnimation>();
	auto& deathStorage = context.GetArrayStorage<CDeath>();
	auto& lifetimeStorage = context.GetArrayStorage<CLifetime>();

	for (auto it = context.Iterate<CHealth, CDeath, CLifetime>(); it.IsValid(); ++it)
	{
		CHealth const& health = healthStorage[it];
		if (!health.GetHealthReachedZero())
		{
			continue;
		}

		// If health is 0 and GetIsDead is false, mark 'died this frame' - then next frame transition 'died this frame' to 'is dead'
		// This is so things can do logic on the frame the entity dies
		CDeath& death = deathStorage[it];
		CAnimation& animation = animStorage[it];
		if (!death.GetIsDead())
		{
			death.SetDiedThisFrame(true);
			death.SetIsDead(true);

			// Start death animation
			if (death.m_deathAnimationName.IsValid())
			{
				// Start death animation
				PlayAnimationRequest request;
				request.m_animGroupName = death.m_deathAnimationName;
				request.m_priority = 100; // High priority so it overrides other animations
				request.m_animSpeedMultiplier = 1.f;
				animation.PlayAnimation(request);
			}

			// Set corpse lifetime
			CLifetime& lifetime = lifetimeStorage[it];
			if (lifetime.m_lifetime < 0.f)
			{
				lifetime.SetLifetime(death.m_corpseDurationSeconds);
			}
		}
		else
		{
			death.SetDiedThisFrame(false);
		}
	}
}
