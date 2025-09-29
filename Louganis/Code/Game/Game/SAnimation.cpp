// Bradley Christensen - 2022-2025
#include "SAnimation.h"
#include "CAnimation.h"
#include "SCCamera.h"
#include "CMovement.h"
#include "CRender.h"
#include "CTransform.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/Constants.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Startup()
{
	AddWriteDependencies<CAnimation, Renderer, AssetManager>();
	AddReadDependencies<CRender, CMovement, CTransform, SCCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Run(SystemContext const& context)
{
	SCCamera& scCamera = g_ecs->GetSingleton<SCCamera>();
	AABB2 cameraBounds = scCamera.m_camera.GetTranslatedOrthoBounds2D();

	auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
	auto& animStorage = g_ecs->GetArrayStorage<CAnimation>();
	auto& movementStorage = g_ecs->GetArrayStorage<CMovement>();

    for (auto it = g_ecs->Iterate<CRender, CAnimation, CTransform>(context); it.IsValid(); ++it)
    {
		CTransform const& transform = transformStorage[it];
        CRender const& render = renderStorage[it];
		if (!GeometryUtils::DoesDiscOverlapAABB(render.GetRenderPosition(), 0.5f * render.m_scale, cameraBounds))
        {
            continue;
        }

		CAnimation& anim = animStorage[it];
        CMovement* movement = movementStorage.Get(it);

		// Initialize sprite sheet and animation instance if not already done
        if (anim.m_gridSpriteSheet == AssetID::Invalid && anim.m_spriteSheetName != Name::Invalid)
        {
            anim.m_gridSpriteSheet = g_assetManager->AsyncLoad<GridSpriteSheet>(anim.m_spriteSheetName);
        }

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
			continue; // Wait until the sprite sheet is loaded before proceeding
        }

        SpriteAnimationDef const* bestAnim = nullptr;

        Vec2 forward = Vec2::MakeFromUnitCircleDegrees(transform.m_orientation);
        bool isMoving = movement ? !movement->m_frameMoveDir.IsZero() : false;
		bool isSprinting = movement ? movement->m_isSprinting : false;
        if (isMoving && isSprinting)
        {
            anim.m_animInstance.SetSpeedMultiplier(movement->m_sprintMoveSpeedMultiplier);
        }
        else
        {
            anim.m_animInstance.SetSpeedMultiplier(1.f);
        }

        if (isMoving)
        {
			SpriteAnimationGroup const* walkGroup = spriteSheet->GetAnimationGroup("walk");
			ASSERT_OR_DIE(walkGroup != nullptr, "SAnimation::Run - Sprite sheet does not have a walk animation group.");
            Vec2 prevDir = anim.m_animInstance.IsValid() ? anim.m_animInstance.GetDef().GetDirection() : Vec2::ZeroVector;
			bestAnim = walkGroup->GetBestAnimForDir(forward, prevDir);
			ASSERT_OR_DIE(bestAnim != nullptr, "SAnimation::Run - No best animation found for direction in walk animation group.");
        }
        else
        {
            SpriteAnimationGroup const* idleGroup = spriteSheet->GetAnimationGroup("idle");
            ASSERT_OR_DIE(idleGroup != nullptr, "SAnimation::Run - Sprite sheet does not have a idle animation group.");
            Vec2 prevDir = anim.m_animInstance.IsValid() ? anim.m_animInstance.GetDef().GetDirection() : Vec2::ZeroVector;
            bestAnim = idleGroup->GetBestAnimForDir(forward, prevDir);
            ASSERT_OR_DIE(bestAnim != nullptr, "SAnimation::Run - No best animation found for direction in idle animation group.");
        }

        if (bestAnim)
        {
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
        }

        anim.m_animInstance.Update(context.m_deltaSeconds);
	}
}
