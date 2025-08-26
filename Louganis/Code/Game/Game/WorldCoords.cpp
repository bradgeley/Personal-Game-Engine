// Bradley Christensen - 2022-2025
#include "WorldCoords.h"
#include "SCWorld.h"



WorldCoords WorldCoords::s_invalidWorldCoords = WorldCoords(IntVec2(INT_MIN, INT_MIN), IntVec2(INT_MIN, INT_MIN));



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
bool WorldCoords::IsValid() const
{
	return *this != s_invalidWorldCoords;
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
