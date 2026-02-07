// Bradley Christensen - 2022-2025
#include "SCamera.h"
#include "SCCamera.h"
#include "WorldSettings.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddWriteDependencies<SCCamera>();

    SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	camera.m_camera.DefineGameSpace(Vec3(0.f, 0.f, -1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();

    Vec2 cameraDims = Vec2(StaticWorldSettings::s_visibleWorldWidth, StaticWorldSettings::s_visibleWorldHeight);
    Vec2 cameraMins = Vec2(StaticWorldSettings::s_cameraMinX, StaticWorldSettings::s_cameraMinY);
    Vec2 cameraMaxs = Vec2(StaticWorldSettings::s_cameraMaxX, StaticWorldSettings::s_cameraMaxY);
    AABB2 cameraBounds = AABB2(cameraMins, cameraMaxs);
    camera.m_camera.SetOrthoBounds2D(cameraBounds);
    camera.m_camera.SetPosition2D(Vec2(0.f, 0.f));
}
