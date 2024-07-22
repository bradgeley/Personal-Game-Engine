// Bradley Christensen - 2023
#include "SDebugRender.h"
#include "CTransform.h"
#include "CPlayerController.h"
#include "SCWorld.h"
#include "WorldRaycast.h"
#include "CCamera.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Startup()
{
    AddWriteDependencies<CTransform, CPlayerController>();
    AddReadDependencies<SCWorld, InputSystem, CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{
    auto transStorage = g_ecs->GetArrayStorage<CTransform>();
    auto cameraStorage = g_ecs->GetMapStorage<CCamera>();
    SCWorld const& world = g_ecs->GetSingleton<SCWorld>();

    for (auto it = g_ecs->Iterate<CTransform, CPlayerController, CCamera>(context); it.IsValid(); ++it)
    {
        CTransform* playerTransform = transStorage.Get(it);
        CCamera* cameraComp = cameraStorage.Get(it);
        Vec2 playerLocation = playerTransform->m_pos;
        Vec2 relMousePos = g_input->GetMouseClientRelativePosition();
        Vec2 worldMousePos = cameraComp->m_camera.ScreenToWorldOrtho(relMousePos);
        Vec2 playerToMouse = worldMousePos - playerLocation;
        WorldRaycast raycast(playerLocation, playerToMouse.GetNormalized(), playerToMouse.GetLength());
        WorldRaycastResult result = Raycast(world, raycast);

        g_renderer->BeginCamera(&cameraComp->m_camera);
        DebugDrawRaycast(result);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Shutdown()
{
    
}