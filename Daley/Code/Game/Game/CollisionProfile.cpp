// Bradley Christensen - 2022-2026
#include "CollisionProfile.h"



//----------------------------------------------------------------------------------------------------------------------
CollisionProfile CollisionProfile::GetDefaultProfile(CollisionChannel objectChannel)
{
	CollisionProfile result;
	result.m_objectChannel = objectChannel;

	switch (objectChannel)
	{
		case CollisionChannel::Projectile:
			result.m_responseChannels = static_cast<CollisionChannel>((uint32_t) CollisionChannel::Enemy);
			break;
		case CollisionChannel::Enemy:
			break;
		case CollisionChannel::Building:
			break;
		case CollisionChannel::GroundEffect:
			result.m_responseChannels = static_cast<CollisionChannel>((uint32_t) CollisionChannel::Enemy);
			break;
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
CollisionProfile CollisionProfile::GetDefaultProfileByName(Name const& name)
{
	if (name == "projectile")
	{
		return GetDefaultProfile(CollisionChannel::Projectile);
	}
	else if (name == "enemy")
	{
		return GetDefaultProfile(CollisionChannel::Enemy);
	}
	else if (name == "building")
	{
		return GetDefaultProfile(CollisionChannel::Building);
	}
	else if (name == "groundeffect")
	{
		return GetDefaultProfile(CollisionChannel::GroundEffect);
	}
	else
	{
		return GetDefaultProfile(CollisionChannel::NoCollision);
	}
}



//----------------------------------------------------------------------------------------------------------------------
CollisionProfile::CollisionProfile(uint32_t objectChannel, uint32_t responseChannels) 
	: m_objectChannel(static_cast<CollisionChannel>(objectChannel)),
	  m_responseChannels(static_cast<CollisionChannel>(responseChannels))
{
}



//----------------------------------------------------------------------------------------------------------------------
CollisionProfile::CollisionProfile(CollisionChannel objectChannel, uint32_t responseChannels)
	: m_objectChannel(objectChannel),
	  m_responseChannels(static_cast<CollisionChannel>(responseChannels))
{
}



//----------------------------------------------------------------------------------------------------------------------
bool CollisionProfile::GetCollisionResponse(CollisionChannel channel) const
{
	return (static_cast<uint32_t>(m_responseChannels) & static_cast<uint32_t>(channel)) != 0;
}
