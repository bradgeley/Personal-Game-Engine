// Bradley Christensen - 2022-2025
#include "SAIController.h"
#include "CTransform.h"
#include "CMovement.h"
#include "CAIController.h"
#include "SCWorld.h"
#include "SCFlowField.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_Z_LAYERS = 2;



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Startup()
{
    AddWriteDependencies<CMovement>();
    AddReadDependencies<CTransform, SCFlowField>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Run(SystemContext const& context)
{
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto& moveStorage = g_ecs->GetArrayStorage<CMovement>();
    auto& scFlow = g_ecs->GetSingleton<SCFlowField>();
	auto& scWorld = g_ecs->GetSingleton<SCWorld>(); // not a true dependency bc we are just calling GetWorldCoordsAtLocation, which is essentially a static function

    for (auto it = g_ecs->Iterate<CTransform, CMovement, CAIController>(context); it.IsValid(); ++it)
    {
        CTransform& transform = *transStorage.Get(it);
        CMovement& movement = *moveStorage.Get(it);
		WorldCoords worldCoords = scWorld.GetWorldCoordsAtLocation(transform.m_pos);
		movement.m_frameMoveDir = scFlow.m_toPlayerFlowField.GetFlowAtWorldCoords(worldCoords);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SAIController::Shutdown()
{

}