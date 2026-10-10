// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class CollisionChannel : uint8_t
{
	NoCollision			= 0,
	Projectile			= 1,
	Enemy				= 2,
	Building			= 3,
	Puddle		= 4,

	Count				= 6,
};



//----------------------------------------------------------------------------------------------------------------------
namespace CollisionChannelUtils
{
	CollisionChannel GetCollisionChannelByName(Name const& name);
	Name GetCollisionChannelName(CollisionChannel channel);
};
