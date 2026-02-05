// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"



//----------------------------------------------------------------------------------------------------------------------
// FlowCoords used in the flow field generation process. Needs to also store distance to the nearest 0 for sorting.
//
class FlowGenerationCoords
{
public:

	FlowGenerationCoords(IntVec2 const& worldCoords, float distance);

public:

	IntVec2 m_tileCoords;
	float m_distance = 0.f;

	bool operator<(FlowGenerationCoords const& rhs) const { return m_distance > rhs.m_distance; }
};

