// Bradley Christensen - 2022-2026
#include "SCCollision.h"

 

//----------------------------------------------------------------------------------------------------------------------
CollisionLayer& SCCollision::GetCollisionLayer(CollisionChannel channel)
{
	return m_collisionLayers[static_cast<uint32_t>(channel)];
}



//----------------------------------------------------------------------------------------------------------------------
CollisionLayer const& SCCollision::GetCollisionLayer(CollisionChannel channel) const
{
	return m_collisionLayers[static_cast<uint32_t>(channel)];
}
