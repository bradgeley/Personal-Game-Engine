// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/FastGrid.h"
#include "Engine/Renderer/RendererUtils.h"
#include "SpawnInfo.h"
#include "TileGeneratedData.h"
#include "WorldSettings.h"
#include <vector>



struct WorldSettings;
class WorldCoords;



//----------------------------------------------------------------------------------------------------------------------
class Chunk
{
public:

	void Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings, std::vector<SpawnInfo>& out_spawnInfos);
	static TileGeneratedData GenerateTileData(IntVec2 const& globalTileCoords, WorldSettings const& worldSettings);
	void Destroy();

	bool IsTileSolid(IntVec2 const& localTileCoords) const;
	bool IsTileSolid(int localTileIndex) const;
	uint8_t GetCost(int localTileIndex) const;

public:

	IntVec2 m_chunkCoords;
	AABB2 m_chunkBounds;
	FastGrid<uint8_t, StaticWorldSettings::s_worldChunkSizePowerOfTwo> m_tileIDs;
	FastGrid<uint8_t, StaticWorldSettings::s_worldChunkSizePowerOfTwo> m_tileLighting; // 4 bits indoor (XXXX0000), 4 outdoor (0000XXXX)
	VertexBufferID m_vbo = RendererUtils::InvalidID;
	std::vector<EntityID> m_spawnedEntities; 
#if defined(_DEBUG)
	VertexBufferID m_debugVBO = RendererUtils::InvalidID;
#endif
};