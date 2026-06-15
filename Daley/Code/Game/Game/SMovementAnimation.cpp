// Bradley Christensen - 2022-2026
#include "SMovementAnimation.h"
#include "CAnimation.h"
#include "CMovement.h"
#include "CRender.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovementAnimation::Startup()
{
    AddReadDependencies<CMovement, CRender>();
    AddWriteDependencies<CAnimation>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovementAnimation::Run(SystemContext const& context) const
{
    // Read Dependencies
    auto& moveStorage = context.GetArrayStorageConst<CMovement>();
	auto& renderStorage = context.GetArrayStorageConst<CRender>();

    // Write Dependencies
    auto& animationStorage = context.GetArrayStorage<CAnimation>();

    static Name idleAnimName = "Idle";
    static Name walkAnimName = "Walk";

    for (auto it = context.Iterate<CMovement, CAnimation, CRender>(); it.IsValid(); ++it)
    {
        CRender const& render = renderStorage[it];
        if (!render.GetIsInCameraView())
        {
            continue;
        }

        CAnimation& anim = animationStorage[it];
        if (anim.m_pendingAnimRequest.m_priority > 1)
        {
			continue; // Higher priority animation is playing
        }

		CMovement const& move = moveStorage[it];
        PlayAnimationRequest request;
        request.m_animGroupName = move.m_frameMoveDir.IsNearlyZero(0.01f) ? idleAnimName : walkAnimName;
        request.m_animSpeedMultiplier = move.m_movementSpeedMultiplier;
        request.m_priority = 1;
        request.m_direction = move.m_frameMoveDir;
        anim.PlayAnimation(request);
    }
}
