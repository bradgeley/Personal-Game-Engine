// Bradley Christensen - 2022-2025
#include "SCamera.h"
#include "SCCamera.h"
#include "CRender.h"
#include "Engine/Window/Window.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddWriteDependencies<SCCamera, Renderer>();
    AddReadDependencies<CRender>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();

    Vec2 cameraDims = Vec2(g_window->GetAspect() * camera.m_baseOrthoHeight, camera.m_baseOrthoHeight);
    Vec2 cameraMins = -cameraDims * 0.5f * camera.m_zoomAmount;
    Vec2 cameraMaxs =  cameraDims * 0.5f * camera.m_zoomAmount;
    AABB2 cameraBounds = AABB2(cameraMins, cameraMaxs);
    camera.m_camera.SetOrthoBounds2D(cameraBounds);
    camera.m_camera.SetPosition2D(Vec2(0.f, 0.f));

    g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
}
