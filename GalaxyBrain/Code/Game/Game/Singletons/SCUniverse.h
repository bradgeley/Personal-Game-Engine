// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Math/IntVec2.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct Star
{
	Vec2 m_pos;
	float m_size;
};



//----------------------------------------------------------------------------------------------------------------------
struct SCUniverse
{
public:

	std::vector<Star> m_parallaxStarsClose;
	std::vector<Star> m_parallaxStarsMid;
	std::vector<Star> m_parallaxStarsFar;
};
