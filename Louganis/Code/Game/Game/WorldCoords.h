#pragma once
#include "Engine/Math/IntVec2.h"



class SCWorld;



//----------------------------------------------------------------------------------------------------------------------
class WorldCoords
{
public:

	WorldCoords() = default;
	WorldCoords(IntVec2 const& chunkCoords, IntVec2 const& localTileCoords);

	IntVec2 GetGlobalTileCoords(int chunkTileWidth) const;

	bool IsValid() const;

	bool operator==(WorldCoords const& other) const;
	bool operator!=(WorldCoords const& other) const;

public:

	IntVec2 m_chunkCoords;
	IntVec2 m_localTileCoords;

public:

	static WorldCoords s_invalidWorldCoords;
};