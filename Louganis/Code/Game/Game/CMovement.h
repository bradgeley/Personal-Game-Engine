// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct CMovement
{
    CMovement() = default;
    CMovement(void const* xmlElement);
    
    // Transient Data
    Vec2 m_frameMoveDir                 = Vec2::ZeroVector;
    Vec2 m_frameMovement                = Vec2::ZeroVector;
    bool m_isSprinting                  = false;
	bool m_isTeleporting                = false;

    // Definition Data
    float m_movementSpeed               = 0.f;
    float m_sprintMoveSpeedMultiplier   = 2.f;
};
