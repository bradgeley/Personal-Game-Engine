// Bradley Christensen - 2022-2026
#include "CollisionChannel.h"



//----------------------------------------------------------------------------------------------------------------------
CollisionChannel CollisionChannelUtils::GetCollisionChannelByName(Name const& name)
{
	if (name == "projectile")
	{
		return CollisionChannel::Projectile;
	}
	else if (name == "enemy")
	{
		return CollisionChannel::Enemy;
	}
	else if (name == "building")
	{
		return CollisionChannel::Building;
	}
	else if (name == "puddle")
	{
		return CollisionChannel::Puddle;
	}
	else
	{
		return CollisionChannel::NoCollision;
	}
}