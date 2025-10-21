﻿// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class MovementFlags : uint8_t
{
    IsSprinting     = 1 << 0,
    IsTeleporting   = 1 << 1,
};



//----------------------------------------------------------------------------------------------------------------------
struct CMovement
{
public:

    CMovement() = default;
    CMovement(void const* xmlElement);

	bool GetIsSprinting() const;
	bool GetIsTeleporting() const;

	void SetIsSprinting(bool isSprinting);
	void SetIsTeleporting(bool isTeleporting);

public:
    
    // Transient Data
    Vec2 m_frameMoveDir                 = Vec2::ZeroVector;
    Vec2 m_frameMovement                = Vec2::ZeroVector;
    float m_movementSpeedMultiplier     = 1.f;
	uint8_t m_movementFlags             = 0;

    // Definition Data
    float m_movementSpeed               = 0.f;
    float m_sprintMoveSpeedMultiplier   = 2.f;
};
