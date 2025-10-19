// Bradley Christensen - 2022-2025
#include "SDeath.h"
#include "CAnimation.h"
#include "CDeath.h"
#include "CHealth.h"
#include "CLifetime.h"
#include "SCEntityFactory.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDeath::Startup()
{
    AddReadDependencies<CHealth>();
	AddWriteDependencies<CAnimation, CDeath, CLifetime>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDeath::Run(SystemContext const& context)
{
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto& deathStorage = g_ecs->GetArrayStorage<CDeath>();
	auto& lifetimeStorage = g_ecs->GetArrayStorage<CLifetime>();
	auto& animStorage = g_ecs->GetArrayStorage<CAnimation>();

	for (auto it = g_ecs->Iterate<CHealth, CDeath, CLifetime>(context); it.IsValid(); ++it)
	{
		CHealth const& health = healthStorage[it];
		if (!health.ShouldDie())
		{
			continue;
		}

		// If health is 0 and GetIsDead is false, mark 'died this frame' - then next frame transition 'died this frame' to 'is dead'
		// This is so things can do logic on the frame the entity dies
		CDeath& death = deathStorage[it];
		CAnimation& animation = animStorage[it];
		if (death.GetDiedThisFrame())
		{
			death.SetDiedThisFrame(false);
			death.SetIsDead(true);
		}
		else if (!death.GetIsDead())
		{
			death.SetDiedThisFrame(true);
		}

		if (death.GetDeathAnimFinishedThisFrame())
		{
			death.SetDeathAnimFinishedThisFrame(false);
			death.SetDeathAnimFinished(true);
		}

		bool diedThisFrame = death.GetDiedThisFrame();
		bool deathAnimFinished = death.GetDeathAnimFinished();

		if (diedThisFrame && !deathAnimFinished)
		{
			if (!death.m_deathAnimationName.IsValid())
			{
				// No death animation, pretend it is finished
				death.SetDeathAnimFinishedThisFrame(true);
			}
			else
			{
				// Start death animation
				PlayAnimationRequest request;
				request.m_animGroupName = death.m_deathAnimationName;
				request.m_priority = 100; // High priority so it overrides other animations
				request.m_animSpeedMultiplier = 1.f;
				animation.PlayAnimation(request);
			}
		}

		if (death.m_deathAnimationName.IsValid())
		{
			ASSERT_OR_DIE(animation.m_pendingAnimRequest.m_animGroupName == death.m_deathAnimationName, "SDeath::Run - Current animation is not death animation. Was it overridden by something?");

			if (animation.m_animInstance.IsFinished() && !deathAnimFinished)
			{
				death.SetDeathAnimFinishedThisFrame(true);
			}
		}
		
		if (death.GetDeathAnimFinishedThisFrame())
		{
			CLifetime& lifetime = lifetimeStorage[it];
			if (lifetime.m_lifetime < 0.f)
			{
				lifetime.SetLifetime(death.m_corpseDurationSeconds);
			}
		}
	}
}
