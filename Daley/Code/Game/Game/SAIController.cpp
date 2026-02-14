// Bradley Christensen - 2022-2025
#include "SAIController.h"
#include "CAIController.h"
#include "CMovement.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Startup()
{
    AddWriteDependencies<CMovement>();
    AddReadDependencies<CTransform, CTime, CAIController, SCFlowField, SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
	auto& timeStorage = g_ecs->GetArrayStorage<CTime>();
	auto& aiStorage = g_ecs->GetArrayStorage<CAIController>();
    SCFlowField const& scFlow = g_ecs->GetSingleton<SCFlowField>();
    SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>(); // not a true dependency bc we are just calling GetWorldCoordsAtLocation, which is essentially a static function

    for (auto it = g_ecs->Iterate<CTransform, CMovement, CAIController>(context); it.IsValid(); ++it)
    {
        CTransform const& transform = *transStorage.Get(it);
        CMovement& movement = *moveStorage.Get(it);
        CTime const& time = *timeStorage.Get(it);
		CAIController const& ai = *aiStorage.Get(it);
        IntVec2 worldCoords = scWorld.GetTileCoordsAtWorldPos(transform.m_pos);
        movement.m_frameMoveDir = scFlow.m_toGoalFlowField.GetFlowAtTileCoords(worldCoords);

        if (ai.GetIsMovementWiggly())
        {
            float maxDegreesOffset = 15.f;
            float wiggliness = 0.15f; // lower = wigglier
            float noiseFactor = maxDegreesOffset * GetPerlinNoise1D(time.m_clock.GetCurrentTimeSecondsF() + static_cast<float>(it.m_currentIndex), wiggliness);

			float additionalDegreeOffset = 15.f;
			float additionalNoiseFactor = additionalDegreeOffset * GetPerlinNoise1D(time.m_clock.GetCurrentTimeSecondsF() + static_cast<float>(it.m_currentIndex) + 1000.f, 10.f);

			Vec2 wigglyMoveDir = movement.m_frameMoveDir.GetRotated(noiseFactor + additionalNoiseFactor);

            movement.m_frameMoveDir = wigglyMoveDir;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Shutdown()
{

}