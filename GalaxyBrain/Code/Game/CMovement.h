// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class MovementMode : uint8_t
{
    Acceleration,
    PolarCoords,
};



//----------------------------------------------------------------------------------------------------------------------
struct CMovement
{
    CMovement() = default;
    CMovement(void const* xmlElement);
    
    MovementMode m_mode = MovementMode::Acceleration;
    Vec2 m_frameMoveDir;
    float m_movementSpeed = 0.f;
};
