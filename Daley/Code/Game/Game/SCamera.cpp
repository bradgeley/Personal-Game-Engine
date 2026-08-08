// Bradley Christensen - 2022-2026
#include "SCamera.h"
#include "SCCamera.h"
#include "SCWindow.h"
#include "SCWorld.h"
#include "WorldSettings.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SetUICameraBounds(SCCamera& camera, IntVec2 const& renderResolution)
{
	float letterboxAspect = StaticWorldSettings::s_visibleWorldAspect;
	IntVec2 viewportDimensions = renderResolution;

	float renderTargetAspect = renderResolution.GetAspect();

	if (renderTargetAspect > letterboxAspect)
	{
		viewportDimensions.x = static_cast<int>(renderResolution.y * letterboxAspect);
	}
	else
	{
		viewportDimensions.y = static_cast<int>(renderResolution.x / letterboxAspect);
	}

	float viewportLeft = (renderResolution.x - viewportDimensions.x) * 0.5f;
	float viewportTop = (renderResolution.y - viewportDimensions.y) * 0.5f;

	camera.m_uiCamera.SetOrthoBounds2D(AABB2(Vec2::ZeroVector, Vec2((float) viewportDimensions.x, (float) viewportDimensions.y)));
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddReadDependencies<SCWindow>();
    AddWriteDependencies<SCCamera>();

	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
    SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	camera.m_worldCamera.DefineGameSpace(Vec3(0.f, 0.f, -1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	camera.m_uiCamera.DefineGameSpace(Vec3(0.f, 0.f, -1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));

    AABB2 cameraBounds = world.GetVisibleWorldBounds();
    camera.m_worldCamera.SetOrthoBounds2D(cameraBounds);
    camera.m_worldCamera.SetPosition2D(Vec2(0.f, 0.f));

	SCWindow const& window = g_ecs->GetSingleton<SCWindow>();
	SetUICameraBounds(camera, window.GetWindow()->GetRenderResolution());
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context) const
{
	SCWindow const& window = context.GetSingletonConst<SCWindow>();
	SCCamera& camera = context.GetSingleton<SCCamera>();

	SetUICameraBounds(camera, window.GetWindow()->GetRenderResolution());
}
