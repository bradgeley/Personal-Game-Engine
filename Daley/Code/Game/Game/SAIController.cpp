// Bradley Christensen - 2022-2025
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

    float timeSeconds = GetCurrentTimeSecondsF();
	constexpr int numPrecomputedNoiseValues = 1279; // prime number to avoid patterns
    float precomputedWiggleNoise[numPrecomputedNoiseValues];
    for (int i = 0; i < numPrecomputedNoiseValues; i++)
    {
        constexpr float maxDegreesOffset = 25.f;
        constexpr float wigglinessScale = 15.f;
        constexpr unsigned int wigglinessOctaves = 2;
        precomputedWiggleNoise[i] = maxDegreesOffset * GetPerlinNoise1D(timeSeconds + static_cast<float>(i), wigglinessScale, wigglinessOctaves);
	}

    for (auto it = g_ecs->Iterate<CTransform, CMovement, CTime, CAIController>(context); it.IsValid(); ++it)
    {
        CTransform const& transform = *transStorage.Get(it);
        CMovement& movement = *moveStorage.Get(it);
		CAIController const& ai = *aiStorage.Get(it);
        IntVec2 worldCoords = scWorld.GetTileCoordsAtWorldPos(transform.m_pos);
        movement.m_frameMoveDir = scFlow.m_toGoalFlowField.GetFlowAtTileCoords(worldCoords);

        if (ai.GetIsMovementWiggly())
        {
			int noiseIndex = static_cast<int>(it.m_currentIndex) % numPrecomputedNoiseValues;
			float noiseValue = precomputedWiggleNoise[noiseIndex];

			Vec2 wigglyMoveDir = movement.m_frameMoveDir.GetRotated(noiseValue);

            movement.m_frameMoveDir = wigglyMoveDir;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Shutdown()
{

}