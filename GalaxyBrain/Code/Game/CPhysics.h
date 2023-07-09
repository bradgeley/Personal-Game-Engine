// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CPhysics
{
    CPhysics() = default;
    CPhysics(void const* xmlElement);
    
    float m_mass = 0.f;
    Vec2 m_frameAcceleration;
    Vec2 m_velocity;
};
