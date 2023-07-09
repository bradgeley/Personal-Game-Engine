// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct Star
{
	void Randomize();

	Vec2 m_pos;
	float m_distance;
	Rgba8 m_tint;
};



//----------------------------------------------------------------------------------------------------------------------
struct SCUniverse
{
public:

	std::vector<Star> m_parallaxStars;
};
