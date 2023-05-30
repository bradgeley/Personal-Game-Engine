// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CMovement : Component
{
    Component* DeepCopy() const override;
    
    Vec2 m_frameMoveDir;
    float m_movementSpeed = 0.f;
};
