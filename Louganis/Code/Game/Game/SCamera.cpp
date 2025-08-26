// Bradley Christensen - 2022-2025
#include "SCamera.h"
#include "CCamera.h"
#include "CRender.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddWriteDependencies<CCamera>();
    AddReadDependencies<CRender>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
    for (auto it = g_ecs->Iterate<CRender, CCamera>(context); it.IsValid(); ++it)
    {
        CRender& render = *g_ecs->GetComponent<CRender>(it);
        CCamera& camera = *g_ecs->GetComponent<CCamera>(it);

        Vec2 cameraDims = Vec2(g_window->GetAspect() * camera.m_baseOrthoHeight, camera.m_baseOrthoHeight);
        Vec2 cameraMins = -cameraDims * 0.5f * camera.m_zoomAmount;
        Vec2 cameraMaxs =  cameraDims * 0.5f * camera.m_zoomAmount;
        AABB2 cameraBounds = AABB2(cameraMins, cameraMaxs);
        camera.m_camera.SetOrthoBounds2D(cameraBounds);
        camera.m_camera.SetPosition2D(render.m_pos);

        g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
    }
}
