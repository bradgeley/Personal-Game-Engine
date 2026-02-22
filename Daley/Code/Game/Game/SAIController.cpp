// Bradley Christensen - 2022-2025
#include "SAIController.h"
#include "CAIController.h"
#include "CMovement.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCTime.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "Engine/Time/Time.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Startup()
{
    AddWriteDependencies<CMovement>();
    AddReadDependencies<CTransform, CTime, CAIController, SCFlowField, SCWorld, SCTime>();
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
	SCTime const& scTime = g_ecs->GetSingleton<SCTime>();
    float timeSeconds = scTime.m_currentTimeSeconds;

    for (auto it = g_ecs->Iterate<CTransform, CMovement, CTime, CAIController>(context); it.IsValid(); ++it)
    {
        CTransform const& transform = *transStorage.Get(it);
        CMovement& movement = *moveStorage.Get(it);
		CAIController const& ai = *aiStorage.Get(it);
        IntVec2 worldCoords = scWorld.GetTileCoordsAtWorldPos(transform.m_pos);
        movement.m_frameMoveDir = scFlow.m_toGoalFlowField.GetFlowAtTileCoords(worldCoords);

        if (ai.GetIsMovementWiggly())
        {
			CTime const& time = *timeStorage.Get(it);

            constexpr float maxDegreesOffset = 25.f;
            constexpr float wigglinessScale = 15.f;
            constexpr unsigned int wigglinessOctaves = 2;
            float angleOffset = maxDegreesOffset * GetPerlinNoise1D(time.m_clock.GetCurrentTimeSecondsF() + static_cast<float>(it.m_currentIndex), wigglinessScale, wigglinessOctaves);

            Vec2 wigglyMoveDir = movement.m_frameMoveDir.GetRotated(angleOffset);
            movement.m_frameMoveDir = wigglyMoveDir;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Shutdown()
{

}