// Bradley Christensen - 2025
#include "WorldCoords.h"
#include "SCWorld.h"



//----------------------------------------------------------------------------------------------------------------------
WorldCoords::WorldCoords(IntVec2 const& chunkCoords, IntVec2 const& localTileCoords)
	: m_chunkCoords(chunkCoords), m_localTileCoords(localTileCoords)
{
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 WorldCoords::GetGlobalTileCoords(int chunkTileWidth) const
{
	return (m_chunkCoords * chunkTileWidth) + m_localTileCoords;
}



//----------------------------------------------------------------------------------------------------------------------
bool WorldCoords::operator==(WorldCoords const& other) const
{
	return (m_chunkCoords == other.m_chunkCoords) && (m_localTileCoords == other.m_localTileCoords);
}



//----------------------------------------------------------------------------------------------------------------------
bool WorldCoords::operator!=(WorldCoords const& other) const
{
	return !(*this == other);
}
