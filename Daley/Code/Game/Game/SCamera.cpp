// Bradley Christensen - 2022-2025
#include "SCamera.h"
#include "SCCamera.h"
#include "WorldSettings.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddWriteDependencies<SCCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();

    Vec2 cameraDims = Vec2(StaticWorldSettings::s_worldWidth, (1.f / g_window->GetAspect()) * StaticWorldSettings::s_worldWidth);
    Vec2 cameraMins = -cameraDims * 0.5f;
    Vec2 cameraMaxs =  cameraDims * 0.5f;
    AABB2 cameraBounds = AABB2(cameraMins, cameraMaxs);
    camera.m_camera.SetOrthoBounds2D(cameraBounds);
    camera.m_camera.SetPosition2D(Vec2(0.f, 0.f));
}
