// Bradley Christensen - 2022-2025
#include "SMovement.h"
#include "CMovement.h"
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform>(context); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
		CTransform& transform = transformStorage[it];

        move.m_frameMovement = move.m_frameMoveDir * move.m_movementSpeed * context.m_deltaSeconds;
        if (move.m_isSprinting)
        {
            move.m_frameMovement *= move.m_sprintMoveSpeedMultiplier;
        }

        if (!move.m_frameMoveDir.IsZero())
        {
            transform.m_orientation = move.m_frameMoveDir.GetAngleDegrees();
        }
    }
}
