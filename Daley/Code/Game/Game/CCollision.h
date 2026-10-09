// Bradley Christensen - 2022-2026
#pragma once
#include "CollisionChannel.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class CollisionFlags : uint8_t
{
	Enabled         = 1 << 0,
	SingleHash      = 1 << 1, // single tile spatial hash, for small entities
};



//----------------------------------------------------------------------------------------------------------------------
struct CCollision
{
public:

    CCollision() = default;
	explicit CCollision(void const* xmlElement);

	bool IsCollisionEnabled() const;
    bool GetIsSingleHash() const;

	void SetCollisionEnabled(bool enabled);
	void SetIsSingleHash(bool singleHash);
    
public:

    float				m_radius				= 0.f;
	CollisionChannel	m_collisionChannel		= CollisionChannel::NoCollision;
    uint8_t				m_collisionFlags		= 1;
	int					m_spatialPartitionTileIndex	= -1; // Which tile this entity is standing in
};
