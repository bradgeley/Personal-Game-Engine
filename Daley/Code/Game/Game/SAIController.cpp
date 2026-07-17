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
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Startup()
{
    AddReadDependencies<CTransform, CAIController, SCFlowField>();
    AddWriteDependencies<CMovement>();

	m_runWhilePaused = false;

    int numThreads = (int) std::thread::hardware_concurrency();
    if (numThreads > 1)
    {
        m_systemSplittingNumJobs = numThreads - 1; // Leave one thread for the main thread to run other systems on
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Run(SystemContext const& context) const
{
	// Read Dependencies
    auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& aiStorage = context.GetArrayStorageConst<CAIController>();
    SCFlowField const& scFlow = context.GetSingletonConst<SCFlowField>();

    // Write Dependencies
    auto& moveStorage = context.GetArrayStorage<CMovement>();

    for (auto it = context.Iterate<CTransform, CMovement, CAIController>(); it.IsValid(); ++it)
    {
        CTransform const& transform = transStorage[it];
		CAIController const& ai = aiStorage[it];
        CMovement& movement = moveStorage[it];

		Vec2 lastFrameMoveDir = movement.m_frameMoveDir;
        IntVec2 worldCoords = SCWorld::GetTileCoordsAtWorldPos(transform.m_pos);
		Vec2 flowDirection = scFlow.m_toGoalFlowField.GetFlowAtTileCoords(worldCoords);

        float angleOffset = 0.f;

        if (ai.GetIsMovementWiggly())
        {
            constexpr float maxDegreesOffset = 25.f;
            constexpr float wigglinessScale = 15.f;
            constexpr unsigned int wigglinessOctaves = 3;
            angleOffset = maxDegreesOffset * GetPerlinNoise1D(Time::GetCurrentTimeSecondsF(), wigglinessScale, wigglinessOctaves, 0.5f, 2.f, true, it.m_currentIndex);

            flowDirection.Rotate(angleOffset);
        }

        if (lastFrameMoveDir.IsNearlyZero())
        {
			lastFrameMoveDir = flowDirection;
        }
		
		float rotationSpeed = movement.m_rotationSpeedDegPerSec; // Rotation speed in degrees per second PER 1 unit/second of movement speed. This normalizes rotation speed where its higher the faster the entity is
        float movementSpeed = movement.m_movementSpeed * movement.m_movementSpeedMultiplier;
		movement.m_frameMoveDir = lastFrameMoveDir.GetRotatedTowards(flowDirection, rotationSpeed * movementSpeed * context.m_deltaSeconds); // Apply movement speed multi to rotation speed
    }
}