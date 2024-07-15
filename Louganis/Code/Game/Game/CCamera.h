// Bradley Christensen - 2023
#pragma once
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
struct CCamera
{
    CCamera() = default;
    CCamera(void const* xmlElement);

    // Transient Data
    Camera m_camera;

    // Definition Data
    float m_tileHeight = 100.f;
};