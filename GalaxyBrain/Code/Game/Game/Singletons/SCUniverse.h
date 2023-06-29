// Bradley Christensen - 2023
#pragma once
#include "Engine/Math/Vec2.h"
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

	std::vector<Star> m_backgroundStars;
};
