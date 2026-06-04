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
			result.m_responseChannels |= GetChannelBit(CollisionChannel::Enemy);
			break;
		case CollisionChannel::Enemy:
			break;
		case CollisionChannel::Building:
			break;
		case CollisionChannel::GroundEffect:
			result.m_responseChannels |= GetChannelBit(CollisionChannel::Enemy);
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
uint32_t CollisionProfile::GetChannelBit(CollisionChannel channel)
{
	return 1 << (uint32_t) channel;
}



//----------------------------------------------------------------------------------------------------------------------
CollisionProfile::CollisionProfile(uint32_t objectChannel, uint32_t responseChannels) 
	: m_objectChannel(static_cast<CollisionChannel>(objectChannel)),
	  m_responseChannels(responseChannels)
{
}



//----------------------------------------------------------------------------------------------------------------------
CollisionProfile::CollisionProfile(CollisionChannel objectChannel, uint32_t responseChannels)
	: m_objectChannel(objectChannel),
	  m_responseChannels(responseChannels)
{
}



//----------------------------------------------------------------------------------------------------------------------
bool CollisionProfile::GetCollisionResponse(CollisionChannel channel) const
{
	uint32_t channelBit = GetChannelBit(channel);
	return (static_cast<uint32_t>(m_responseChannels) & static_cast<uint32_t>(channelBit)) != 0;
}
