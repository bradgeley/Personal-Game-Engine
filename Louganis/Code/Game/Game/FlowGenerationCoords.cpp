// Bradley Christensen - 2022-2025
#include "FlowGenerationCoords.h"



//----------------------------------------------------------------------------------------------------------------------
FlowGenerationCoords::FlowGenerationCoords(IntVec2 const& chunkCoords, IntVec2 const& localTileCoords, float distance)
	: WorldCoords(chunkCoords, localTileCoords), m_distance(distance)
{
}



//----------------------------------------------------------------------------------------------------------------------
FlowGenerationCoords::FlowGenerationCoords(WorldCoords const& worldCoords, float distance) : WorldCoords(worldCoords), m_distance(distance)
{
}
