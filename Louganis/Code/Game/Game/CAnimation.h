// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Assets/Sprites/GridSpriteSheet.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct CAnimation
{
    CAnimation() = default;
    CAnimation(void const* xmlElement);

	// todo: move to AssetManager
	GridSpriteSheet m_spriteSheet;

	// todo: move to animation
	bool m_pingpong = false; // change to sprite anim type
	bool m_isPingponging = false;
	std::vector<int> m_animationFrames = { 0, 1, 2 };
	int m_currentFrame = 0;
	float m_secondsPerFrame = 0.f;
	float m_timeAccumulated = 0.f;
};
