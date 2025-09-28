// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/EntityID.h"
#include "Engine/Events/EventDelegate.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/FastGrid.h"
#include "Engine/Renderer/RendererUtils.h"
#include "SpawnInfo.h"
#include "Tile.h"
#include "TileGeneratedData.h"
#include "WorldSettings.h"
#include <vector>



struct WorldSettings;
class WorldCoords;
class Image;



//----------------------------------------------------------------------------------------------------------------------
class Chunk
{
public:

	void Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings, std::vector<SpawnInfo>& out_spawnInfos);
	void GenerateVBO();
	void GenerateLightmap();
	void GenerateLightmapImage(Image& out_image);
	static TileGeneratedData GenerateTileData(IntVec2 const& globalTileCoords, WorldSettings const& worldSettings);
	void Destroy();

	bool IsTileSolid(IntVec2 const& localTileCoords) const;
	bool IsTileSolid(int localTileIndex) const;
	uint8_t GetCost(int localTileIndex) const;

public:

	IntVec2 m_chunkCoords;
	AABB2 m_chunkBounds;
	bool m_isLightingDirty = true;
	FastGrid<Tile, StaticWorldSettings::s_worldChunkSizePowerOfTwo> m_tiles;
	VertexBufferID m_vbo = RendererUtils::InvalidID;
	TextureID m_lightmap = RendererUtils::InvalidID; // R8G8
	std::vector<EntityID> m_spawnedEntities; 
#if defined(_DEBUG)
	VertexBufferID m_debugVBO = RendererUtils::InvalidID;
#endif
};