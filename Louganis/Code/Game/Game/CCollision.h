// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class CollisionFlags : uint8_t
{
    Immovable = 1 << 0,
};


//----------------------------------------------------------------------------------------------------------------------
struct CCollision
{
    CCollision() = default;
    CCollision(void const* xmlElement);

    bool IsImmovable() const;
    
    float   m_radius            = 0.f;
	Vec2    m_offset            = Vec2::ZeroVector;
    uint8_t m_collisionFlags    = 0;
};
