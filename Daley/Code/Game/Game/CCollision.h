// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec2.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class CollisionFlags : uint8_t
{
	Enabled         = 1 << 0,
    Immovable       = 1 << 1,
	SingleHash      = 1 << 2 // single tile spatial hash, for small entities
};



//----------------------------------------------------------------------------------------------------------------------
struct CCollision
{
public:

    CCollision() = default;
    CCollision(void const* xmlElement);

	bool IsCollisionEnabled() const;
    bool IsImmovable() const;
    bool GetIsSingleHash() const;

	void SetCollisionEnabled(bool enabled);
	void SetImmovable(bool immovable);
	void SetIsSingleHash(bool singleHash);
    
public:

    float   m_radius            = 0.f;
	Vec2    m_offset            = Vec2::ZeroVector;
    uint8_t m_collisionFlags    = 1;
};
