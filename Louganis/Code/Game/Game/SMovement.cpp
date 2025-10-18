// Bradley Christensen - 2022-2025
#include "SMovement.h"
#include "CMovement.h"
#include "CTransform.h"
#include "CHealth.h"
#include "CTime.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddReadDependencies<CTime, CHealth>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& timeStorage = g_ecs->GetMapStorage<CTime>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform, CTime>(context); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
		CTransform& transform = transformStorage[it];

        if (move.m_isTeleporting)
        {
            transform.m_pos += move.m_frameMovement;
            move.m_frameMovement = Vec2::ZeroVector;
            continue;
        }

        CHealth* health = g_ecs->GetComponent<CHealth>(it.m_currentIndex);
        if (health && health->GetIsDead())
        {
            continue;
        }

        CTime& time = timeStorage[it];
        move.m_frameMovement = move.m_frameMoveDir * move.m_movementSpeed * move.m_movementSpeedMultiplier * time.m_clock.GetDeltaSecondsF();
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
