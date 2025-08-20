// Bradley Christensen - 2023
#include "SMovement.h"
#include "CMovement.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CMovement>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();

    for (auto it = g_ecs->Iterate<CMovement>(context); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];

        move.m_frameMovement = move.m_frameMoveDir * move.m_movementSpeed * context.m_deltaSeconds;
        if (move.m_isSprinting)
        {
            move.m_frameMovement *= move.m_sprintMoveSpeedMultiplier;
        }
    }
}
