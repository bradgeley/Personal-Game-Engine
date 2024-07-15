// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct CMovement
{
    CMovement() = default;
    CMovement(void const* xmlElement);
    
    // Transient Data
    Vec2 m_frameMoveDir = Vec2::ZeroVector;

    // Definition Data
    float m_movementSpeed = 0.f;
};
