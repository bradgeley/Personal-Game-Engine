// Bradley Christensen - 2022-2025
#include "SPhysics.h"
#include "CCollision.h"
#include "CMovement.h"
#include "CTransform.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddReadDependencies<CCollision>();

    int numThreads = std::thread::hardware_concurrency() - 1;
    m_systemSplittingNumJobs = numThreads - 1;
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform, CCollision>(context); it.IsValid(); ++it)
    {
        CCollision const& collision = collisionStorage[it];
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
