// Bradley Christensen - 2025
#pragma once
#include "WorldCoords.h"



//----------------------------------------------------------------------------------------------------------------------
// FlowCoords used in the flow field generation process. Needs to also store distance to the nearest 0 for sorting.
//
class FlowGenerationCoords : public WorldCoords
{
public:

	FlowGenerationCoords(IntVec2 const& chunkCoords, IntVec2 const& localTileCoords, float distance);
	FlowGenerationCoords(WorldCoords const& worldCoords, float distance);

public:

	float m_distance = 0.f;

	bool operator<(FlowGenerationCoords const& rhs) const { return m_distance > rhs.m_distance; }
};

