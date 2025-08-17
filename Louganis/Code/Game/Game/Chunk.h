// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/FastGrid.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Events/EventDelegate.h"
#include "WorldSettings.h"



struct WorldSettings;
class WorldCoords;



//----------------------------------------------------------------------------------------------------------------------
class Chunk
{
public:

	void Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings);
	void Destroy();

	bool IsTileSolid(IntVec2 const& localTileCoords) const;
	bool IsTileSolid(int tileIndex) const;
	uint8_t GetCost(int tileIndex) const;

public:

	EventDelegate m_destroyed;
	IntVec2 m_chunkCoords;
	AABB2 m_chunkBounds;
	FastGrid<uint8_t, s_worldChunkSizePowerOfTwo> m_tileIDs;
	VertexBuffer m_vbo;
#if defined(_DEBUG)
	VertexBuffer m_debugVBO;
#endif
};