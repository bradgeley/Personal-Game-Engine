// Bradley Christensen - 2023
#include "SCollision.h"
#include "Game/Game/Components/CCollision.h"
#include "Game/Game/Components/CTransform.h"
#include "Game/Game/Components/CPhysics.h"
#include "Engine/Input/InputSystem.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddWriteDependencies<CTransform, CPhysics, CCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const& context)
{
    for (auto it = g_ecs->Iterate<CTransform, CCollision>(context); it.IsValid(); ++it)
    {

    }
}
