// Bradley Christensen - 2022-2026
#include "SCamera.h"
#include "GameCommon.h"
#include "SCCamera.h"
#include "SCWindow.h"
#include "WorldSettings.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void UpdateCameraZoom(SystemContext const& context);
void UpdateCameraPan(SystemContext const& context);
void SetUICameraBounds(SCCamera& camera, IntVec2 const& renderResolution, bool letterboxed);



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Startup()
{
    AddReadDependencies<SCWindow>();
    AddWriteDependencies<SCCamera>();

    SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
	SCWindow const& window = g_ecs->GetSingleton<SCWindow>();
	float windowAspect = window.GetWindow()->GetAspect();

	Vec3 gameForward	= Vec3(0.f, 0.f, -1.f);
	Vec3 gameLeft		= Vec3(-1.f, 0.f, 0.f);
	Vec3 gameUp			= Vec3(0.f, 1.f, 0.f);

	// Same for UI as game
	camera.m_worldCamera.DefineGameSpace(gameForward, gameLeft, gameUp);
	camera.m_uiCamera.DefineGameSpace(gameForward, gameLeft, gameUp);
	
	AABB2 bounds = AABB2(0.f, 0.f, StaticGameSettings::s_baseCameraHeightInTiles * windowAspect, StaticGameSettings::s_baseCameraHeightInTiles);
	bounds.SetCenter(Vec2::ZeroVector);
	camera.m_worldCamera.SetOrthoBounds2D(bounds);

	SetUICameraBounds(camera, window.GetWindow()->GetRenderResolution(), false);
}



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context) const
{
	UpdateCameraZoom(context);
	UpdateCameraPan(context);
}




//----------------------------------------------------------------------------------------------------------------------
void UpdateCameraZoom(SystemContext const& context)
{
	SCCamera& camera = context.GetSingleton<SCCamera>();
	if (camera.m_frameZoomDelta == 0)
	{
		return;
	}

	while (camera.m_frameZoomDelta != 0)
	{
		// 1 step at a time

		float zoomDirection = MathUtils::Sign((float) camera.m_frameZoomDelta);
		Vec2 cameraDims = camera.m_worldCamera.GetOrthoDimensions2D();
		float newCameraHeight = cameraDims.y - (zoomDirection * StaticGameSettings::s_cameraZoomRatioPerStep * cameraDims.y);
		newCameraHeight = MathUtils::Clamp(newCameraHeight, StaticGameSettings::s_minCameraHeight, StaticGameSettings::s_maxCameraHeight);
		float newCameraWidth = cameraDims.x * (newCameraHeight / cameraDims.y);

		if (camera.m_clampVisionToVisibleWorld)
		{
			if (newCameraHeight > StaticWorldSettings::s_numVisibleWorldTilesY)
			{
				float cameraShrinkRatio = StaticWorldSettings::s_numVisibleWorldTilesY / newCameraHeight;
				newCameraHeight = StaticWorldSettings::s_numVisibleWorldTilesY;
				newCameraWidth *= cameraShrinkRatio;
			}
			if (newCameraWidth > StaticWorldSettings::s_numVisibleWorldTilesX)
			{
				float cameraShrinkRatio = StaticWorldSettings::s_numVisibleWorldTilesX / newCameraWidth;
				newCameraWidth = StaticWorldSettings::s_numVisibleWorldTilesX;
				newCameraHeight *= cameraShrinkRatio;
			}
		}
		else
		{
			if (newCameraHeight > StaticWorldSettings::s_numPlayableWorldTilesY)
			{
				float cameraShrinkRatio = StaticWorldSettings::s_numPlayableWorldTilesY / newCameraHeight;
				newCameraHeight = StaticWorldSettings::s_numPlayableWorldTilesY;
				newCameraWidth *= cameraShrinkRatio;
			}
			if (newCameraWidth > StaticWorldSettings::s_numPlayableWorldTilesX)
			{
				float cameraShrinkRatio = StaticWorldSettings::s_numPlayableWorldTilesX / newCameraWidth;
				newCameraWidth = StaticWorldSettings::s_numPlayableWorldTilesX;
				newCameraHeight *= cameraShrinkRatio;
			}
		}

		if (MathUtils::IsNearlyEqual(newCameraHeight, cameraDims.y) && MathUtils::IsNearlyEqual(newCameraWidth, cameraDims.x))
		{
			// Hit min or max zoom, stop zooming
			camera.m_frameZoomDelta = 0;
			break;
		}

		Vec2 worldFocalPoint = camera.m_worldCamera.ScreenToWorldOrtho(camera.m_relativeZoomFocusPoint);
		Vec2 translation = worldFocalPoint - camera.m_worldCamera.GetTranslatedOrthoBounds2D().GetCenter();
		translation *= zoomDirection;
		translation *= StaticGameSettings::s_cameraZoomRatioPerStep;
		camera.m_worldCamera.Translate2D(translation);

		if (camera.m_frameZoomDelta > 0)
		{
			--camera.m_frameZoomDelta;
		}
		else
		{
			++camera.m_frameZoomDelta;
		}

		camera.m_worldCamera.SetOrthoDims2D(Vec2(newCameraWidth, newCameraHeight));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void UpdateCameraPan(SystemContext const& context)
{
	SCCamera& camera = context.GetSingleton<SCCamera>();

	float cameraHeight = camera.m_worldCamera.GetOrthoDimensions().y;
	Vec2 panDirection = camera.m_framePanDirection;
	camera.m_framePanDirection = Vec2::ZeroVector;
	float panSpeed = StaticGameSettings::s_cameraHeightRatioPanSpeed * cameraHeight;
	camera.m_worldCamera.Translate2D(panDirection * panSpeed * context.GetRealTimeDeltaSeconds());

	AABB2 visibleWorldBounds;
	if (camera.m_clampVisionToVisibleWorld)
	{
		visibleWorldBounds = AABB2(StaticWorldSettings::s_visibleWorldMinsX, StaticWorldSettings::s_visibleWorldMinsY, StaticWorldSettings::s_visibleWorldMaxsX, StaticWorldSettings::s_visibleWorldMaxsY);
	}
	else
	{
		visibleWorldBounds = AABB2(StaticWorldSettings::s_playableWorldMinsX, StaticWorldSettings::s_playableWorldMinsY, StaticWorldSettings::s_playableWorldMaxsX, StaticWorldSettings::s_playableWorldMaxsY);
	}

	AABB2 cameraBounds = camera.m_worldCamera.GetTranslatedOrthoBounds2D();
	Vec2 previousCenter = cameraBounds.GetCenter();
	cameraBounds.ClampInside(visibleWorldBounds);
	Vec2 clampedCenter = cameraBounds.GetCenter();
	Vec2 translation = clampedCenter - previousCenter;
	camera.m_worldCamera.Translate2D(translation);
}



//----------------------------------------------------------------------------------------------------------------------
void SetUICameraBounds(SCCamera& camera, IntVec2 const& renderResolution, bool letterboxed)
{
	if (!letterboxed)
	{
		camera.m_uiCamera.SetOrthoBounds2D(AABB2(Vec2::ZeroVector, Vec2(renderResolution)));
		return;
	}

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