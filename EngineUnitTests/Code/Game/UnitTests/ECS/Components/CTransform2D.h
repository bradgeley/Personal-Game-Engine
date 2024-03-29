﻿// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CTransform2D : Component
{
    CTransform2D() = default;
    CTransform2D(float x, float y, float orientationDegrees = 0.f);
    CTransform2D(Vec2 const& v, float orientationDegrees = 0.f);
    CTransform2D(CTransform2D const& other );
    
    Component* DeepCopy() const override;

    Vec2 m_screenPosition = Vec2::ZeroVector;
    float m_orientationDegrees = 0.f;
};
