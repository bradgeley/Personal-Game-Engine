// Bradley Christensen - 2022-2025
#include "SAnimation.h"
#include "CAnimation.h"
#include "CRender.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Startup()
{
	AddWriteDependencies<CAnimation, AssetManager>();
	AddReadDependencies<CRender>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Run(SystemContext const& context)
{
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
	auto& animStorage = g_ecs->GetArrayStorage<CAnimation>();

    for (auto it = g_ecs->Iterate<CRender, CAnimation>(context); it.IsValid(); ++it)
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
			defaultRequest.m_animGroupName = "idle";
			defaultRequest.m_priority = 0;
			defaultRequest.m_animSpeedMultiplier = 1.f;
			defaultRequest.m_direction = Vec2::MakeFromUnitCircleDegrees(render.m_orientation);
            anim.PlayAnimation(defaultRequest);
		}

        if (anim.m_gridSpriteSheet == AssetID::Invalid && anim.m_spriteSheetName != Name::Invalid)
        {
            anim.m_gridSpriteSheet = g_assetManager->AsyncLoad<GridSpriteSheet>(anim.m_spriteSheetName);
        }

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
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
