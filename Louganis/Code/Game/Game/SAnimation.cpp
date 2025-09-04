// Bradley Christensen - 2022-2025
#include "SAnimation.h"
#include "CAnimation.h"
#include "CMovement.h"
#include "CTransform.h"
#include "Engine/Assets/Sprites/GridSpriteSheet.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/Constants.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Startup()
{
	AddWriteDependencies<CAnimation, Renderer, AssetManager>();
	AddReadDependencies<CMovement, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAnimation::Run(SystemContext const& context)
{
    for (auto it = g_ecs->Iterate<CAnimation, CMovement>(context); it.IsValid(); ++it)
    {
        CAnimation& anim = *g_ecs->GetComponent<CAnimation>(it);
        CMovement& movement = *g_ecs->GetComponent<CMovement>(it);
		CTransform& transform = *g_ecs->GetComponent<CTransform>(it);

		// Initialize sprite sheet and animation instance if not already done
        if (anim.m_gridSpriteSheet == INVALID_ASSET_ID)
        {
            anim.m_gridSpriteSheet = g_assetManager->AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/Soldier.xml"); // todo: move asset path to entity def
        }

        if (g_input->WasKeyJustPressed(KeyCode::Space))
        {
            g_assetManager->AsyncReload<GridSpriteSheet>(anim.m_gridSpriteSheet);
        }

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
			continue; // Wait until the sprite sheet is loaded before proceeding
        }

        if (!anim.m_animInstance.IsValid())
        {
            anim.m_animInstance = spriteSheet->GetAnimationDef("southIdle")->MakeAnimInstance(); // todo: move default/starting anim to entity def
        }

        Vec2 forward = Vec2::MakeFromUnitCircleDegrees(transform.m_orientation);
        bool isMoving = !movement.m_frameMoveDir.IsZero();
        if (!isMoving)
        {
            if (forward.Dot(Vec2(0, -1)) >= MathConstants::ROOT_2_OVER_2)
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("southIdle")), true);
            }
            else if (forward.Dot(Vec2(0, 1)) >= MathConstants::ROOT_2_OVER_2)
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("northIdle")), true);
            }
            else if (forward.Dot(Vec2(1, 0)) >= MathConstants::ROOT_2_OVER_2)
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("eastIdle")), true);
            }
            else
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("westIdle")), true);
			}
        }
        else
        {
            if (forward.Dot(Vec2(0, -1)) >= MathConstants::ROOT_2_OVER_2)
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("southWalk")), false);
            }
            else if (forward.Dot(Vec2(0, 1)) >= MathConstants::ROOT_2_OVER_2)
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("northWalk")), false);
            }
            else if (forward.Dot(Vec2(1, 0)) >= MathConstants::ROOT_2_OVER_2)
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("eastWalk")), false);
            }
            else
            {
                anim.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("westWalk")), false);
            }

            if (movement.m_isSprinting)
            {
                anim.m_animInstance.SetSpeedMultiplier(movement.m_sprintMoveSpeedMultiplier);
            }
            else
            {
                anim.m_animInstance.SetSpeedMultiplier(1.f);
            }
        }


		anim.m_animInstance.Update(context.m_deltaSeconds);
	}
}
