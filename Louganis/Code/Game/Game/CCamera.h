// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
struct CCamera
{
    CCamera() = default;
    CCamera(void const* xmlElement);

    // Transient Data
    Camera m_camera;
    float m_zoomAmount = 1.f;
	bool m_isActive = false; // only 1 camera may be active at a time in the game world

    // Definition Data
    float m_minZoom = 0.1f;
    float m_maxZoom = 100.f;
    float m_zoomMultiplier = 0.05f;
    float m_baseOrthoHeight = 100.f;
};