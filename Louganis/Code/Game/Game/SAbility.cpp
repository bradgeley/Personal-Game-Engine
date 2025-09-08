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

    }
}

 

//----------------------------------------------------------------------------------------------------------------------
void SAbility::Shutdown()
{

}
