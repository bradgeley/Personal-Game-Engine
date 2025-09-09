// Bradley Christensen - 2022-2025
#include "SCamera.h"
#include "CCamera.h"
#include "CPlayerController.h"
#include "CRender.h"
#include "Engine/Window/Window.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddWriteDependencies<CCamera>();
    AddReadDependencies<CRender, CPlayerController>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
    bool cameraActive = false;
    for (auto it = g_ecs->Iterate<CPlayerController, CCamera>(context); it.IsValid(); ++it)
    {
        // Set the first player camera we find to be active, and disable all other cameras
		CCamera& camera = *g_ecs->GetComponent<CCamera>(it);
        camera.m_isActive = !cameraActive;
        cameraActive = true;
    }


    for (auto it = g_ecs->Iterate<CRender, CCamera>(context); it.IsValid(); ++it)
    {
        CCamera& camera = *g_ecs->GetComponent<CCamera>(it);
        if (!camera.m_isActive)
        {
            continue;
        }

        CRender& render = *g_ecs->GetComponent<CRender>(it);

        Vec2 cameraDims = Vec2(g_window->GetAspect() * camera.m_baseOrthoHeight, camera.m_baseOrthoHeight);
        Vec2 cameraMins = -cameraDims * 0.5f * camera.m_zoomAmount;
        Vec2 cameraMaxs =  cameraDims * 0.5f * camera.m_zoomAmount;
        AABB2 cameraBounds = AABB2(cameraMins, cameraMaxs);
        camera.m_camera.SetOrthoBounds2D(cameraBounds);
        camera.m_camera.SetPosition2D(render.m_pos);

        g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
    }
}
