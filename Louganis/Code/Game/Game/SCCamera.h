// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
struct SCCamera
{
	EntityID m_attachedEntity = ENTITY_ID_INVALID; // Entity that the camera should follow, if any

    // Transient Data
    Camera m_camera;
    float m_zoomAmount = 1.f;

    // Definition Data
    float m_minZoom = 0.1f;
    float m_maxZoom = 100.f;
    float m_zoomMultiplier = 0.05f;
    float m_baseOrthoHeight = 100.f;
};