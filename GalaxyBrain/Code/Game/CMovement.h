// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CMovement
{
    CMovement() = default;
    CMovement(void const* xmlElement);
    
    Vec2 m_frameMoveDir;
    float m_movementSpeed = 0.f;
};
