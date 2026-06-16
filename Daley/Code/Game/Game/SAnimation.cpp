// Bradley Christensen - 2022-2026
#include "SAnimation.h"
#include "CAnimation.h"
#include "CRender.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/StringUtils.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Startup()
{
	AddReadDependencies<CRender>();
	AddWriteDependencies<AssetManager, CAnimation>();

	int numThreads = std::thread::hardware_concurrency();
    if (numThreads > 1)
    {
		SetSystemSplittingNumJobs(numThreads - 1);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::PreRun(SystemContext const& context) const
{
    // Pull async load calls into PreRun so that Run can be split

    auto& animStorage = context.GetArrayStorage<CAnimation>();

    for (auto it = context.Iterate<CAnimation>(); it.IsValid(); ++it)
    {
        // Initialize sprite sheet and animation instance if not already done
        CAnimation& anim = animStorage[it];

        if (anim.m_gridSpriteSheet == AssetID::Invalid && anim.m_spriteSheetName != Name::Invalid)
        {
            // Released in CAnimation destructor (must have whole ECS write access for components to destruct, so should be ok)
            anim.m_gridSpriteSheet = g_assetManager->AsyncLoad<GridSpriteSheet>(anim.m_spriteSheetName);
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Run(SystemContext const& context) const
{
    // Read Dependencies
	auto& renderStorage = context.GetArrayStorageConst<CRender>();
    // g_assetManager

    // Write Dependencies
	auto& animStorage = context.GetArrayStorage<CAnimation>();

    for (auto it = context.Iterate<CRender, CAnimation>(); it.IsValid(); ++it)
    {
        CRender const& render = renderStorage[it];
        if (!render.GetIsInCameraView())
        {
            continue;
        }

		// Initialize sprite sheet and animation instance if not already done
        CAnimation& anim = animStorage[it];

        if (anim.m_pendingAnimRequest.m_animGroupName == Name::Invalid)
        {
			PlayAnimationRequest defaultRequest;
			defaultRequest.m_animGroupName = anim.m_defaultAnimationName;
			defaultRequest.m_priority = 0;
			defaultRequest.m_animSpeedMultiplier = 1.f;
			defaultRequest.m_direction = Vec2::MakeFromUnitCircleDegrees(render.m_orientation);
            anim.PlayAnimation(defaultRequest);
		}

        GridSpriteSheet const* spriteSheet = g_assetManager->Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
			continue; // Wait until the sprite sheet is loaded before proceeding
        }

        if (anim.m_animInstance.IsValid() && anim.m_animInstance.GetDef().GetType() == SpriteAnimationType::SingleFrame && spriteSheet->GetNumAnimations() == 1)
        {
			// Early out for trees and other single-animation sprite sheets
            continue;
        }

        SpriteAnimationGroup const* animGroup = spriteSheet->GetAnimationGroup(anim.m_pendingAnimRequest.m_animGroupName);
        ASSERT_OR_DIE(animGroup != nullptr, StringUtils::StringF("SAnimation::Run - Sprite sheet does not have a %s animation.", anim.m_pendingAnimRequest.m_animGroupName.ToCStr()));

        Vec2 prevDir = anim.m_animInstance.IsValid() ? anim.m_animInstance.GetDef().GetDirection() : Vec2::ZeroVector;
        SpriteAnimationDef const*  bestAnim = animGroup->GetBestAnimForDir(anim.m_pendingAnimRequest.m_direction, prevDir);
        ASSERT_OR_DIE(bestAnim != nullptr, StringUtils::StringF("SAnimation::Run - No animation found for dir in %s animation group.", anim.m_pendingAnimRequest.m_animGroupName.ToCStr()));

        if (anim.m_animInstance.IsValid())
        {
            Name currentGroup = anim.m_animInstance.GetDef().GetAnimGroupName();
            bool shouldRestart = (bestAnim->GetAnimGroupName() != currentGroup);
            anim.m_animInstance.ChangeDef(*bestAnim, shouldRestart);
        }
        else
        {
            anim.m_animInstance = bestAnim->MakeAnimInstance(0, 1);
        }

        if (anim.m_animInstance.IsValid())
        {
			anim.m_animInstance.SetSpeedMultiplier(anim.m_pendingAnimRequest.m_animSpeedMultiplier);
            anim.m_animInstance.Update(context.m_deltaSeconds);
		}
	}
}
