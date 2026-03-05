// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct CAttachment
{
    CAttachment() = default;
    
	EntityID m_attachedTo = EntityID::Invalid;
	bool m_destroyIfAttachedToEntityDestroyed = false;

	EntityID m_attachedBurnVFX = EntityID::Invalid;
};
