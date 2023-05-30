// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CPhysics : Component
{
    Component* DeepCopy() const override;
    
    float m_gravityStrength = 0.f;
    Vec2 m_frameAcceleration;
    Vec2 m_velocity;
};
