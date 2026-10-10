// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
struct SCCamera
{
    Vec2 m_framePanDirection = Vec2::ZeroVector;
	int m_frameZoomDelta = 0;
	Vec2 m_relativeZoomFocusPoint = Vec2::ZeroVector; // 0,0 = bot left 1,1 = top right of client

    bool m_clampVisionToVisibleWorld = true;

    Camera m_worldCamera;
    Camera m_uiCamera;
};