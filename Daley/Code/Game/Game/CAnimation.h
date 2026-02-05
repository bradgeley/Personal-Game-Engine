// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/SpriteAnimation.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct PlayAnimationRequest
{
	Name	m_animGroupName			= Name::Invalid;
	int		m_priority				= 0;
	float	m_animSpeedMultiplier	= 1.f;
	Vec2	m_direction				= Vec2::ZeroVector;
	bool	m_clearWhenFinished		= false; // if true, clears the anim request after its finished
};



//----------------------------------------------------------------------------------------------------------------------
struct CAnimation
{
public:

    CAnimation() = default;
    CAnimation(void const* xmlElement);

	bool PlayAnimation(PlayAnimationRequest const& request, bool force = false);

public:

	PlayAnimationRequest m_pendingAnimRequest;			// Queued animation

	Name m_spriteSheetName;								// Name of the sprite sheet used for this entity
	AssetID m_gridSpriteSheet = AssetID::Invalid;		// Asset ID of the grid sprite sheet used for this animation
	SpriteAnimation m_animInstance;						// currently playing animation
};
