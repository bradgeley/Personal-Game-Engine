// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/SpriteAnimation.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct CAnimation
{
    CAnimation() = default;
    CAnimation(void const* xmlElement);

	Name m_spriteSheetName;								// Name of the sprite sheet used for this entity
	AssetID m_gridSpriteSheet = AssetID::Invalid;		// Asset ID of the grid sprite sheet used for this animation
	SpriteAnimation m_animInstance;						// currently playing animation
};
