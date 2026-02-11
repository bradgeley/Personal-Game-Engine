// Bradley Christensen - 2022-2025
#include "SCamera.h"
#include "SCCamera.h"
#include "SCWorld.h"
#include "WorldSettings.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	camera.m_camera.DefineGameSpace(Vec3(0.f, 0.f, -1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));

    AABB2 cameraBounds = world.GetVisibleWorldBounds();
    camera.m_camera.SetOrthoBounds2D(cameraBounds);
    camera.m_camera.SetPosition2D(Vec2(0.f, 0.f));
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const&)
{
    // empty, camera doesnt move
}
