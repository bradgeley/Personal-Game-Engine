// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CPhysics
{
    CPhysics() = default;
    CPhysics(void const* xmlElement);
    
    float m_mass = 0.f;
    float m_angularVelocity = 0.f;
    Vec2 m_frameAcceleration;
    Vec2 m_velocity;

    // For moving around planets with polar coordinates
    Vec2 m_polarVelocity; // x = degrees about center, y = radius
};
