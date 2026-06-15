// Bradley Christensen - 2022-2026
#include "SAIController.h"
#include "CAIController.h"
#include "CMovement.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "Engine/Time/Time.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Startup()
{
    AddReadDependencies<CTransform, CTime, CAIController, SCFlowField>();
    AddWriteDependencies<CMovement>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Run(SystemContext const& context) const
{
	// Read Dependencies
    auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& timeStorage = context.GetArrayStorageConst<CTime>();
	auto& aiStorage = context.GetArrayStorageConst<CAIController>();
    SCFlowField const& scFlow = context.GetSingletonConst<SCFlowField>();

    // Write Dependencies
    auto& moveStorage = context.GetArrayStorage<CMovement>();

    for (auto it = context.Iterate<CTransform, CMovement, CTime, CAIController>(); it.IsValid(); ++it)
    {
        CTransform const& transform = transStorage[it];
		CAIController const& ai = aiStorage[it];
        CMovement& movement = moveStorage[it];

        IntVec2 worldCoords = SCWorld::GetTileCoordsAtWorldPos(transform.m_pos);
        movement.m_frameMoveDir = scFlow.m_toGoalFlowField.GetFlowAtTileCoords(worldCoords);

        if (ai.GetIsMovementWiggly())
        {
			CTime const& time = timeStorage[it];

            constexpr float maxDegreesOffset = 25.f;
            constexpr float wigglinessScale = 15.f;
            constexpr unsigned int wigglinessOctaves = 2;
            float angleOffset = maxDegreesOffset * GetPerlinNoise1D(time.m_clock.GetCurrentTimeSecondsF() + static_cast<float>(it.m_currentIndex), wigglinessScale, wigglinessOctaves);

            Vec2 wigglyMoveDir = movement.m_frameMoveDir.GetRotated(angleOffset);
            movement.m_frameMoveDir = wigglyMoveDir;
        }
    }
}