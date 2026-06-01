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
enum class CollisionObjectType : uint8_t
{
	Projectile		= 1 << 0,
	Enemy			= 1 << 1,
	Building		= 1 << 2,
	AoEEffect		= 1 << 3,

	Invalid			= 1 << 4,
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

	bool GetCollisionResponse(CollisionObjectType otherType) const;
    
public:

    float   m_radius							= 0.f;
	Vec2    m_offset							= Vec2::ZeroVector;
    uint8_t m_collisionFlags					= 1;
	uint8_t m_collisionObjectResponses			= 0;
	CollisionObjectType m_collisionObjectType	= CollisionObjectType::Invalid;
};
