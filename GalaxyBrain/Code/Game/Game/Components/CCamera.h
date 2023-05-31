// Bradley Christensen - 2023
#pragma once
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
struct CCamera
{
    CCamera() = default;
    CCamera(void const* xmlElement);

    Vec2 m_baseDims = Vec2(100.f, 50.f);
    float m_zoom = 1.f;
    Camera m_camera;
};