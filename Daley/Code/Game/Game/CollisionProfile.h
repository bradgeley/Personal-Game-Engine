// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
constexpr uint32_t MAX_COLLISION_CHANNELS = 32;



//----------------------------------------------------------------------------------------------------------------------
enum class CollisionChannel : uint32_t
{
	NoCollision			= 0,
	Projectile			= 1 << 0,
	Enemy				= 1 << 1,
	Building			= 1 << 2,
	GroundEffect		= 1 << 3,
};



//----------------------------------------------------------------------------------------------------------------------
// Collision Profile
//
// What object type I am, and what other object types do I respond to.
//
struct CollisionProfile
{
	CollisionProfile() = default;
	CollisionProfile(uint32_t objectChannel, uint32_t responseChannels);
	CollisionProfile(CollisionChannel objectChannel, uint32_t responseChannels);

	bool GetCollisionResponse(CollisionChannel channel) const;

	static CollisionProfile GetDefaultProfile(CollisionChannel objectChannel);
	static CollisionProfile GetDefaultProfileByName(Name const& name);

	CollisionChannel m_objectChannel	= CollisionChannel::NoCollision;
	CollisionChannel m_responseChannels = CollisionChannel::NoCollision;
};