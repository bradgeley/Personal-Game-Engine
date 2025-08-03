// Bradley Christensen - 2025
#include "FlowGenerationCoords.h"



//----------------------------------------------------------------------------------------------------------------------
FlowGenerationCoords::FlowGenerationCoords(IntVec2 const& chunkCoords, IntVec2 const& localTileCoords, float distance)
	: WorldCoords(chunkCoords, localTileCoords), m_distance(distance)
{
}
