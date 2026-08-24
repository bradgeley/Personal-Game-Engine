// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>
#include <string>



struct EntityDebugContext;



//----------------------------------------------------------------------------------------------------------------------
enum class MovementFlags : uint8_t
{
	ConstrainedToPath   = 1 << 0,
	MovedThisFrame      = 1 << 1
};



//----------------------------------------------------------------------------------------------------------------------
struct CMovement
{
public:

    CMovement() = default;
    CMovement(void const* xmlElement);

    void AppendDebugString(EntityDebugContext& context) const;

	bool GetIsConstrainedToPath() const;
    bool GetMovedThisFrame() const;

	void SetIsConstrainedToPath(bool isConstrainedToPath);
    void SetMovedThisFrame(bool movedThisFrame);

public:
    
    // Transient Data
    Vec2 m_frameMoveDir                 = Vec2::ZeroVector;
    Vec2 m_frameMovement                = Vec2::ZeroVector;
    float m_movementSpeedMultiplier     = 1.f;
	uint8_t m_movementFlags             = 0;

    // Definition Data
    float m_movementSpeed               = 0.f;
    float m_rotationSpeedDegPerSec      = 90.f;
};
