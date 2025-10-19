﻿// Bradley Christensen - 2022-2025
#include "SMovement.h"
#include "CMovement.h"
#include "CTransform.h"
#include "CDeath.h"
#include "CTime.h"



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddWriteDependencies<CMovement, CTransform>();
    AddReadDependencies<CTime, CDeath>();
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& deathStorage = g_ecs->GetArrayStorage<CDeath>();
    auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& timeStorage = g_ecs->GetArrayStorage<CTime>();

	BitMask deathBitMask = g_ecs->GetComponentBitMask<CDeath>();

    for (auto it = g_ecs->Iterate<CMovement, CTransform, CTime>(context); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
        if (g_ecs->DoesEntityHaveComponents(it.m_currentIndex, deathBitMask))
        {
            CDeath const& death = deathStorage[it];
            if (death.GetIsDead())
            {
                move.m_frameMoveDir = Vec2::ZeroVector;
                move.m_frameMovement = Vec2::ZeroVector;
                move.m_isTeleporting = false;
                continue;
            }
        }

		CTransform& transform = transformStorage[it];

        if (move.m_isTeleporting)
        {
            transform.m_pos += move.m_frameMovement;
            move.m_frameMovement = Vec2::ZeroVector;
            continue;
        }

        CTime const& time = timeStorage[it];
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
