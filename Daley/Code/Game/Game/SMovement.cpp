// Bradley Christensen - 2022-2026
#include "SMovement.h"
#include "CMovement.h"
#include "CTransform.h"
#include "CDeath.h"
#include "CTime.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
    AddReadDependencies<CTime, CDeath>();
    AddWriteDependencies<CMovement, CTransform>();

    int numThreads = (int) std::thread::hardware_concurrency();
    if (numThreads > 1)
    {
        m_systemSplittingNumJobs = numThreads - 1; // Leave one thread for the main thread to run other systems on
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context) const
{
    // Read Dependencies
    auto& timeStorage = context.GetArrayStorageConst<CTime>();
    auto& deathStorage = context.GetArrayStorageConst<CDeath>();

	// Write Dependencies
    auto& moveStorage = context.GetArrayStorage<CMovement>();
    auto& transformStorage = context.GetArrayStorage<CTransform>();

	BitMask deathBitMask = g_ecs->GetComponentBitMask<CDeath>();

    for (auto it = context.Iterate<CMovement, CTransform, CTime>(); it.IsValid(); ++it)
    {
        CMovement& move = moveStorage[it];
        if (g_ecs->DoesEntityHaveComponentsUnsafe(it.m_currentIndex, deathBitMask))
        {
            CDeath const& death = deathStorage[it];
            if (death.GetIsDead())
            {
                move.m_frameMoveDir = Vec2::ZeroVector;
                move.m_frameMovement = Vec2::ZeroVector;
                continue;
            }
        }

        CTime const& time = timeStorage[it];
        move.m_frameMovement = move.m_frameMoveDir * move.m_movementSpeed * move.m_movementSpeedMultiplier * context.m_deltaSeconds * time.m_clock.GetTimeDilationF();

        if (!move.m_frameMoveDir.IsZero())
        {
            CTransform& transform = transformStorage[it];
            transform.m_orientation = move.m_frameMoveDir.GetAngleDegrees();
        }
    }
}
