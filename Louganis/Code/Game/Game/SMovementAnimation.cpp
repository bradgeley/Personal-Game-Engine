// Bradley Christensen - 2022-2025
#include "SMovementAnimation.h"
#include "CAnimation.h"
#include "CMovement.h"
#include "CRender.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovementAnimation::Startup()
{
    AddWriteDependencies<CAnimation>();
    AddReadDependencies<CMovement, CRender>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovementAnimation::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& animationStorage = g_ecs->GetArrayStorage<CAnimation>();
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();

    static Name idleAnimName = "Idle";
    static Name walkAnimName = "Walk";

    for (auto it = g_ecs->Iterate<CMovement, CAnimation, CRender>(context); it.IsValid(); ++it)
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
        request.m_animSpeedMultiplier = move.GetIsSprinting() ? move.m_sprintMoveSpeedMultiplier : 1.f;
        request.m_priority = 1;
        request.m_direction = move.m_frameMoveDir;
        anim.PlayAnimation(request);
    }
}
