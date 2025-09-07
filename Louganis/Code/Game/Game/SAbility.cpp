// Bradley Christensen - 2022-2025
#include "SAbility.h"
#include "CAbility.h"
#include "CAnimation.h"
#include "CMovement.h"
#include "CTransform.h"
#include "Engine/Math/Constants.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Startup()
{
    AddReadDependencies<CTransform, AssetManager>();
    AddWriteDependencies<CAbility, CAnimation, CMovement>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAbility::Run(SystemContext const& context)
{
    for (auto it = g_ecs->Iterate<CAbility, CAnimation, CTransform>(context); it.IsValid(); ++it)
    {
		CAbility& ability = *g_ecs->GetComponent<CAbility>(it);
		CAnimation& animation = *g_ecs->GetComponent<CAnimation>(it);
		CTransform& transform = *g_ecs->GetComponent<CTransform>(it);
		CMovement& movement = *g_ecs->GetComponent<CMovement>(it);

        Vec2 forward = Vec2::MakeFromUnitCircleDegrees(transform.m_orientation);

        bool wantsToCast = ability.m_wasCastButtonJustPressed;
        bool justStartedCasting = false;
        if (!ability.m_isCastingAbility && wantsToCast)
        {
            ability.m_isCastingAbility = true;
            justStartedCasting = true;
		}

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(animation.m_gridSpriteSheet);
        if (!spriteSheet)
        {
            ability.m_isCastingAbility = false;
            continue;
        }

        if (!animation.m_animInstance.IsValid())
        {
            ability.m_isCastingAbility = false;
            continue;
        }

        if (ability.m_isCastingAbility)
        {
            if (animation.m_animInstance.IsFinished())
            {
                ability.m_isCastingAbility = false;
            }
        }

        if (ability.m_isCastingAbility)
        {
            movement.m_movementSpeedMultiplier = 0.25f;
		}
        else
        {
			movement.m_movementSpeedMultiplier = 1.f;
        }

        if (justStartedCasting)
        {
            if (forward.Dot(Vec2(0, -1)) >= MathConstants::ROOT_2_OVER_2)
            {
                animation.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("southAttack1")), true);
            }
            else if (forward.Dot(Vec2(0, 1)) >= MathConstants::ROOT_2_OVER_2)
            {
                animation.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("northAttack1")), true);
            }
            else if (forward.Dot(Vec2(1, 0)) >= MathConstants::ROOT_2_OVER_2)
            {
                animation.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("eastAttack1")), true);
            }
            else
            {
                animation.m_animInstance.ChangeDef(*spriteSheet->GetAnimationDef(Name("westAttack1")), true);
            }
        }
    }
}

 

//----------------------------------------------------------------------------------------------------------------------
void SAbility::Shutdown()
{

}
