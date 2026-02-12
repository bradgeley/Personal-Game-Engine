// Bradley Christensen - 2022-2025
#include "SPhysics.h"
#include "CMovement.h"
#include "CTransform.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform>(context); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
        CTransform& transform = transStorage[it];

		transform.m_pos += move.m_frameMovement;
		move.m_frameMovement = Vec2::ZeroVector;
    }
}

 

//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Shutdown()
{

}
