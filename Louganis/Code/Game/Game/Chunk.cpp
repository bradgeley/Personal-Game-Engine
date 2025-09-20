// Bradley Christensen - 2022-2025
#include "Chunk.h"
#include "WorldSettings.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "TileDef.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr float MOUNTAIN_TERRAIN_HEIGHT			= 0.75f;
constexpr float GRASS_TERRAIN_HEIGHT			= 0.5f;
constexpr float SAND_TERRAIN_HEIGHT				= 0.45f;
constexpr float SHALLOW_WATER_TERRAIN_HEIGHT	= 0.4f;
constexpr float WATER_TERRAIN_HEIGHT			= 0.35f;
constexpr float DEEP_WATER_TERRAIN_HEIGHT		= 0.0f;



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings)
{
	m_chunkCoords = chunkCoords;
	int numTilesInRow = MathUtils::Pow(2, StaticWorldSettings::s_worldChunkSizePowerOfTwo);
	int numTilesInChunk = numTilesInRow * numTilesInRow;
	float chunkWidth = StaticWorldSettings::s_tileWidth * numTilesInRow;
	Vec2 chunkOrigin = Vec2(chunkCoords.x, chunkCoords.y) * chunkWidth;
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	m_chunkBounds = AABB2(chunkOrigin, chunkOrigin + tileDims * (float) numTilesInRow);

	int grassTileDef = TileDef::GetTileDefID("grass");
	int wallTileDef  = TileDef::GetTileDefID("wall");
	int waterTileDef  = TileDef::GetTileDefID("water");
	int shallowWaterTileDef  = TileDef::GetTileDefID("shallowWater");
	int deepWaterTileDef  = TileDef::GetTileDefID("deepWater");
	int sandTileDef  = TileDef::GetTileDefID("sand");
	TileDef const* errorTileDef = TileDef::GetTileDef("error");

	// Generate tile IDs
	m_tileIDs.Initialize(IntVec2(numTilesInRow, numTilesInRow), (uint8_t) grassTileDef);

	Vec2 chunkMinsTileCoords = Vec2(chunkCoords) * (float) numTilesInRow;

	// Generate walls with perlin noise
	for (int y = 0; y < numTilesInRow; ++y)
	{
		for (int x = 0; x < numTilesInRow; ++x)
		{
			// Convert Local XY to World XY
			Vec2 worldTileCoords = chunkMinsTileCoords + Vec2(x, y);

			int index = m_tileIDs.GetIndexForCoords(x, y);
			float terrainHeightNoise = GetPerlinNoise2D_01(worldTileCoords.x, worldTileCoords.y, 100.f, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
			float moistureNoise = GetPerlinNoise2D_01(worldTileCoords.x, worldTileCoords.y, 2500.f, 2, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 1));
			float mountainNoise = GetPerlinNoise2D_01(worldTileCoords.x, worldTileCoords.y, 2500.f, 2, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 2));

			terrainHeightNoise += mountainNoise;
			terrainHeightNoise *= moistureNoise;

			if (terrainHeightNoise >= MOUNTAIN_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) wallTileDef);
			}
			else if (terrainHeightNoise >= GRASS_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) grassTileDef);
			}
			else if (terrainHeightNoise >= SAND_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) sandTileDef);
			}
			else if (terrainHeightNoise >= SHALLOW_WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) shallowWaterTileDef);
			}
			else if (terrainHeightNoise >= WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) waterTileDef);
			}
			else // (terrainHeightNoise >= DEEP_WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) deepWaterTileDef);
			}
		}
	}

	// Generate Vbo
	m_vbo = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);

	AssetID terrainID = g_assetManager->LoadSynchronous<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	GridSpriteSheet const* terrainSpriteSheet = g_assetManager->Get<GridSpriteSheet>(terrainID);
	ASSERT_OR_DIE(terrainSpriteSheet != nullptr, "Chunk::Generate - Failed to load terrain sprite sheet");

	vbo.Initialize<Vertex_PCU>(6 * numTilesInChunk);
	for (int y = 0; y < numTilesInRow; ++y)
	{
		for (int x = 0; x < numTilesInRow; ++x)
		{
			int index = m_tileIDs.GetIndexForCoords(x, y);
			uint8_t tileID = m_tileIDs.Get(index);
			TileDef const* tileDef = TileDef::GetTileDef(tileID);
			if (!tileDef)
			{
				tileDef = errorTileDef;
			}
			Rgba8 const& tint = tileDef->m_tint;
			Vec2 mins = chunkOrigin + Vec2(x, y) * StaticWorldSettings::s_tileWidth;
			Vec2 maxs = mins + tileDims;
			AABB2 uvs = terrainSpriteSheet->GetSpriteUVs(tileDef->m_spriteIndex);
			VertexUtils::AddVertsForRect2D(vbo, mins, maxs, tint, uvs);
		}
	}

	g_assetManager->Release(terrainID);

	#if defined(_DEBUG)
		m_debugVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
		VertexBuffer& debugVBO = *g_renderer->GetVertexBuffer(m_debugVBO);
		VertexUtils::AddVertsForWireGrid(debugVBO, m_chunkBounds, IntVec2(numTilesInRow, numTilesInRow), 0.01f, Rgba8::Black);
		VertexUtils::AddVertsForWireBox2D(debugVBO, m_chunkBounds, 0.03f, Rgba8::Red);
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Destroy()
{
	m_tileIDs.Clear();

	g_renderer->ReleaseVertexBuffer(m_vbo);
	#if defined(_DEBUG)
		g_renderer->ReleaseVertexBuffer(m_debugVBO);
	#endif

	NamedProperties args;
	m_destroyed.Broadcast(args);
}



//----------------------------------------------------------------------------------------------------------------------
bool Chunk::IsTileSolid(IntVec2 const& localTileCoords) const
{
	uint8_t tileID = m_tileIDs.Get(localTileCoords);
	TileDef const* tileDef = TileDef::GetTileDef(tileID);
	if (tileDef)
	{
		return tileDef->IsSolid();
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool Chunk::IsTileSolid(int localTileIndex) const
{
	uint8_t tileID = m_tileIDs.Get(localTileIndex);
	TileDef const* tileDef = TileDef::GetTileDef(tileID);
	if (tileDef)
	{
		return tileDef->IsSolid();
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
uint8_t Chunk::GetCost(int localTileIndex) const
{
	uint8_t tileID = m_tileIDs.Get(localTileIndex);
	TileDef const* tileDef = TileDef::GetTileDef(tileID);
	if (tileDef)
	{
		return tileDef->m_cost;
	}
	return (uint8_t)255;
}
