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
	Projectile			= 1,
	Enemy				= 2,
	Building			= 3,
	GroundEffect		= 4,

	Count				= 5,
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
	static uint32_t GetChannelBit(CollisionChannel channel);

	CollisionChannel m_objectChannel = CollisionChannel::NoCollision;
	uint32_t m_responseChannels	= 0;
};