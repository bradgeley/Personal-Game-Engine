#pragma once
#include "Engine/Math/IntVec2.h"



class SCWorld;



//----------------------------------------------------------------------------------------------------------------------
class WorldCoords
{
public:

	WorldCoords() = default;
	WorldCoords(IntVec2 const& chunkCoords, IntVec2 const& localTileCoords);

	IntVec2 GetGlobalTileCoords() const;

	bool IsValid() const;

	bool operator==(WorldCoords const& other) const;
	bool operator!=(WorldCoords const& other) const;

public:

	IntVec2 m_chunkCoords;
	IntVec2 m_localTileCoords;

public:

	static WorldCoords s_invalidWorldCoords;
};



//----------------------------------------------------------------------------------------------------------------------
namespace std
{
	template<>
	struct hash<WorldCoords>
	{
		std::size_t operator()(const WorldCoords& wc) const noexcept
		{
			std::size_t h1 = std::hash<int>()(wc.m_chunkCoords.x) ^ (std::hash<int>()(wc.m_chunkCoords.y) << 1);
			std::size_t h2 = std::hash<int>()(wc.m_localTileCoords.x) ^ (std::hash<int>()(wc.m_localTileCoords.y) << 1);
			return h1 ^ (h2 << 1);
		}
	};
}