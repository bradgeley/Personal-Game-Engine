// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/EntityID.h"



//----------------------------------------------------------------------------------------------------------------------
// PlanetGenerator
// 
// Dependencies: All (creates entities)
//
class PlanetGenerator
{
public:

	EntityID Generate() const;

	float m_minRadius = 1.f;
	float m_maxRadius = 1.f;
};