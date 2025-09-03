// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Sprites/GridSpriteSheet.h"
#include "Engine/Assets/Sprites/SpriteAnimation.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct CAnimation
{
    CAnimation() = default;
    CAnimation(void const* xmlElement);

	// todo: move to AssetManager
	GridSpriteSheet m_spriteSheet;
	SpriteAnimation m_animation;
};
