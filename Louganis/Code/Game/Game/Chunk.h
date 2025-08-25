// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/FastGrid.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Events/EventDelegate.h"
#include "WorldSettings.h"



struct WorldSettings;
class WorldCoords;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
class Chunk
{
public:

	void Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings);
	void Destroy();

	bool IsTileSolid(IntVec2 const& localTileCoords) const;
	bool IsTileSolid(int localTileIndex) const;
	uint8_t GetCost(int localTileIndex) const;

public:

	EventDelegate m_destroyed;
	IntVec2 m_chunkCoords;
	AABB2 m_chunkBounds;
	FastGrid<uint8_t, s_worldChunkSizePowerOfTwo> m_tileIDs;
	VertexBuffer* m_vbo = nullptr;
#if defined(_DEBUG)
	VertexBuffer* m_debugVBO = nullptr;
#endif
};