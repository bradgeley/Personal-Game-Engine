// Bradley Christensen - 2022-2025
#include "SAIController.h"
#include "CTransform.h"
#include "CMovement.h"
#include "CAIController.h"
#include "SCWorld.h"
#include "SCFlowField.h"



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Startup()
{
    AddWriteDependencies<CMovement>();
    AddReadDependencies<CTransform, SCFlowField, SCWorld>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    SCFlowField const& scFlow = g_ecs->GetSingleton<SCFlowField>();
    SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>(); // not a true dependency bc we are just calling GetWorldCoordsAtLocation, which is essentially a static function

	WorldCoords playerWorldCoords = scWorld.m_lastKnownPlayerWorldCoords;
    Vec2 playerWorldPos = scWorld.GetTileBounds(playerWorldCoords).GetCenter();

    for (auto it = g_ecs->Iterate<CTransform, CMovement, CAIController>(context); it.IsValid(); ++it)
    {
        CTransform const& transform = *transStorage.Get(it);
        float distSquaredToPlayer = playerWorldPos.GetDistanceSquaredTo(transform.m_pos);
        if (distSquaredToPlayer > StaticWorldSettings::s_flowFieldGenerationRadiusSquared)
        {
            // AI go into 'sleep' mode when outside the flow radius
            continue;
		}

        CMovement& movement = *moveStorage.Get(it);
		WorldCoords worldCoords = scWorld.GetWorldCoordsAtLocation(transform.m_pos);
		movement.m_frameMoveDir = scFlow.m_toPlayerFlowField.GetFlowAtWorldCoords(worldCoords);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Shutdown()
{

}