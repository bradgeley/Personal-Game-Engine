// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class CollisionFlags : uint8_t
{
	Enabled         = 1 << 0,
    Immovable       = 1 << 1,
};



//----------------------------------------------------------------------------------------------------------------------
struct CCollision
{
public:

    CCollision() = default;
    CCollision(void const* xmlElement);

	bool IsCollisionEnabled() const;
    bool IsImmovable() const;

	void SetCollisionEnabled(bool enabled);
	void SetImmovable(bool immovable);
    
public:

    float   m_radius            = 0.f;
	Vec2    m_offset            = Vec2::ZeroVector;
    uint8_t m_collisionFlags    = 1;
};
