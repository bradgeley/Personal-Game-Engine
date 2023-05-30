﻿// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/Renderer/Camera.h"



//----------------------------------------------------------------------------------------------------------------------
struct CCamera : Component
{
    Component* DeepCopy() const override;
    
    Camera m_camera;
};