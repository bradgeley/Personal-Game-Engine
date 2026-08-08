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

	camera.m_uiCamera.SetOrthoBounds2D(AABB2(Vec2::ZeroVector, Vec2((float) viewportDimensions.x, (float) viewportDimensions.y)));
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddReadDependencies<SCWindow>();
    AddWriteDependencies<SCCamera>();

	SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
	SCWindow const& window = g_ecs->GetSingleton<SCWindow>();
    SCCamera& camera = g_ecs->GetSingleton<SCCamera>();

	Vec3 gameForward	= Vec3(0.f, 0.f, -1.f);
	Vec3 gameLeft		= Vec3(-1.f, 0.f, 0.f);
	Vec3 gameUp			= Vec3(0.f, 1.f, 0.f);

	// Same for UI as game
	camera.m_worldCamera.DefineGameSpace(gameForward, gameLeft, gameUp);
	camera.m_uiCamera.DefineGameSpace(gameForward, gameLeft, gameUp);

    AABB2 cameraBounds = world.GetVisibleWorldBounds();
    camera.m_worldCamera.SetOrthoBounds2D(cameraBounds);
    camera.m_worldCamera.SetPosition2D(Vec2(0.f, 0.f));

	SetUICameraBounds(camera, window.GetWindow()->GetRenderResolution());
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context) const
{
	SCWindow const& window = context.GetSingletonConst<SCWindow>();
	SCCamera& camera = context.GetSingleton<SCCamera>();

	SetUICameraBounds(camera, window.GetWindow()->GetRenderResolution());
}
