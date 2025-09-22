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
constexpr float SEA_LEVEL						= 0.5f;
constexpr float MOUNTAIN_TERRAIN_HEIGHT			= 0.65f;
constexpr float GRASS_TERRAIN_HEIGHT			= 0.5f;
constexpr float SAND_TERRAIN_HEIGHT				= 0.49f;
constexpr float SHALLOW_WATER_TERRAIN_HEIGHT	= 0.4f;
constexpr float WATER_TERRAIN_HEIGHT			= 0.35f;
constexpr float DEEP_WATER_TERRAIN_HEIGHT		= 0.0f;
constexpr float DESERT_HUMIDITY_THRESHOLD		= 0.1f;



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings, std::vector<SpawnInfo>& out_spawnInfos)
{
	m_chunkCoords = chunkCoords;
	Vec2 chunkOrigin = Vec2(chunkCoords.x, chunkCoords.y) * StaticWorldSettings::s_chunkWidth;
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	m_chunkBounds = AABB2(chunkOrigin, chunkOrigin + tileDims * StaticWorldSettings::s_numTilesInRowF);

	int grassTileDef = TileDef::GetTileDefID("grass");
	int forestGrassTileDef = TileDef::GetTileDefID("forestGrass");
	int deepForestGrassTileDef = TileDef::GetTileDefID("deepForestGrass");
	int wallTileDef  = TileDef::GetTileDefID("wall");
	int waterTileDef  = TileDef::GetTileDefID("water");
	int shallowWaterTileDef  = TileDef::GetTileDefID("shallowWater");
	int deepWaterTileDef  = TileDef::GetTileDefID("deepWater");
	int sandTileDef  = TileDef::GetTileDefID("sand");
	TileDef const* errorTileDef = TileDef::GetTileDef("error");

	// Generate tile IDs
	m_tileIDs.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), (uint8_t) grassTileDef);

	Vec2 chunkMinsWorldLocation = Vec2(chunkCoords) * StaticWorldSettings::s_chunkWidth;

	for (int y = 0; y < StaticWorldSettings::s_numTilesInRow; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x)
		{
			Vec2 worldTileLoc = chunkMinsWorldLocation + (Vec2(x, y) * tileDims);
			TileGeneratedData tileGenData = GenerateTileData(worldTileLoc, worldSettings);; // could cache this off but would increase the memory usage of each chunk by an order of magnitude or so

			int index = m_tileIDs.GetIndexForCoords(x, y);

			if (tileGenData.m_terrainHeight >= MOUNTAIN_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) wallTileDef);
			}
			else if (tileGenData.m_terrainHeight >= GRASS_TERRAIN_HEIGHT)
			{
				if (tileGenData.m_forestness > 0.75)
				{
					m_tileIDs.Set(index, (uint8_t) deepForestGrassTileDef);
				}
				else if (tileGenData.m_forestness > 0.5f)
				{
					m_tileIDs.Set(index, (uint8_t) forestGrassTileDef);
				}
				else
				{
					m_tileIDs.Set(index, (uint8_t) grassTileDef);
				}
			}
			else if (tileGenData.m_terrainHeight >= SAND_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) sandTileDef);
			}
			else if (tileGenData.m_terrainHeight >= SHALLOW_WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) shallowWaterTileDef);
			}
			else if (tileGenData.m_terrainHeight >= WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) waterTileDef);
			}
			else // (tileGenData.m_terrainHeight >= DEEP_WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) deepWaterTileDef);
			}

			if (tileGenData.m_humidity < DESERT_HUMIDITY_THRESHOLD && tileGenData.m_terrainHeight >= GRASS_TERRAIN_HEIGHT && tileGenData.m_terrainHeight < MOUNTAIN_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) sandTileDef);
			}
		}
	}

	// Generate Vbo
	m_vbo = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);

	AssetID terrainID = g_assetManager->LoadSynchronous<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	GridSpriteSheet const* terrainSpriteSheet = g_assetManager->Get<GridSpriteSheet>(terrainID);
	ASSERT_OR_DIE(terrainSpriteSheet != nullptr, "Chunk::Generate - Failed to load terrain sprite sheet");

	vbo.Initialize<Vertex_PCU>(6 * StaticWorldSettings::s_numTilesInChunk);
	for (int y = 0; y < StaticWorldSettings::s_numTilesInRow; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x)
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
TileGeneratedData Chunk::GenerateTileData(Vec2 const& worldTileLocation, WorldSettings const& worldSettings) const
{
	TileGeneratedData tileGenData;

	// Humidity
	tileGenData.m_humidity = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_humidityScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 1));
	tileGenData.m_humidity = MathUtils::SmoothStep3(tileGenData.m_humidity);
	tileGenData.m_humidity *= -1.f; // Invert so higher moisture is lower value
	tileGenData.m_humidity = MathUtils::RangeMapClamped(tileGenData.m_humidity, -1.f, 0.f, 0.f, 0.5f);

	// Mountainness
	tileGenData.m_mountainness = GetPerlinNoise2D(worldTileLocation.x, worldTileLocation.y, worldSettings.m_mountainnessScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 2));
	tileGenData.m_mountainness = MathUtils::AbsF(tileGenData.m_mountainness);
	tileGenData.m_mountainness = MathUtils::SmoothStep3(tileGenData.m_mountainness);

	// Oceanness
	tileGenData.m_oceanness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_oceannessScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 3));
	tileGenData.m_oceanness = MathUtils::SmoothStep3(tileGenData.m_oceanness);

	// Forestness
	tileGenData.m_forestness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_forestnessScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4));
	tileGenData.m_forestness = MathUtils::SmoothStep3(tileGenData.m_forestness);

	// Riverness
	float rivernessScale = worldSettings.m_rivernessScale;//(1.f - tileGenData.m_humidity) * worldSettings.m_rivernessScale + worldSettings.m_rivernessScale * 0.5f;
	tileGenData.m_riverness = GetPerlinNoise2D(worldTileLocation.x, worldTileLocation.y, rivernessScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 5));
	tileGenData.m_riverness = MathUtils::AbsF(tileGenData.m_riverness);

	// Terrain Height
	tileGenData.m_terrainHeightOffset = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_terrainHeightOffsetScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
	tileGenData.m_terrainHeightOffset *= tileGenData.m_mountainness;

	// River runs through here
	float riverMaxDepth = MathUtils::RangeMapClamped(tileGenData.m_humidity, 1.f, 0.f, worldSettings.m_riverMaxDepth, SEA_LEVEL);
	float riverThreshold = MathUtils::RangeMapClamped(tileGenData.m_humidity, 1.f, 0.f, worldSettings.m_riverThreshold, 0.f);

	if (tileGenData.m_riverness < riverThreshold)
	{
		tileGenData.m_terrainHeightOffset = MathUtils::RangeMapClamped(tileGenData.m_riverness, riverThreshold, 0.f, -0.05f, riverMaxDepth - SEA_LEVEL);
	}

	if (tileGenData.m_oceanness > worldSettings.m_oceanDeepWaterThreshold)
	{
		tileGenData.m_terrainHeightOffset = DEEP_WATER_TERRAIN_HEIGHT - SEA_LEVEL;
	}
	else if (tileGenData.m_oceanness > worldSettings.m_oceanWaterThreshold)
	{
		tileGenData.m_terrainHeightOffset = WATER_TERRAIN_HEIGHT - SEA_LEVEL;
	}
	else if (tileGenData.m_oceanness > worldSettings.m_oceanShallowWaterThreshold)
	{
		tileGenData.m_terrainHeightOffset = SHALLOW_WATER_TERRAIN_HEIGHT - SEA_LEVEL;
	}

	tileGenData.m_terrainHeight = SEA_LEVEL + tileGenData.m_terrainHeightOffset;

	if (tileGenData.m_oceanness > worldSettings.m_oceanShallowWaterThreshold && tileGenData.m_terrainHeight >= WATER_TERRAIN_HEIGHT && tileGenData.m_riverness < riverThreshold)
	{
		// Handle rivers transitioning into ocean
		float riverHeight = SEA_LEVEL + MathUtils::RangeMapClamped(tileGenData.m_riverness, riverThreshold, 0.f, -0.05f, riverMaxDepth - SEA_LEVEL);
		float oceanHeight = tileGenData.m_terrainHeight;
		tileGenData.m_terrainHeight = MathUtils::InterpolateClamped(riverHeight, oceanHeight, MathUtils::RangeMapClamped(tileGenData.m_oceanness, worldSettings.m_oceanShallowWaterThreshold, 1.f, 0.f, worldSettings.m_riverToOceanTransitionSpeed));
	}
	else if (tileGenData.m_humidity < DESERT_HUMIDITY_THRESHOLD && tileGenData.m_riverness < riverThreshold)
	{
		// Handle rivers transitioning into desert
		float riverHeight = SEA_LEVEL + MathUtils::RangeMapClamped(tileGenData.m_riverness, riverThreshold, 0.f, -0.05f, riverMaxDepth - SEA_LEVEL);
		float desertHeight = 0.55f;
		tileGenData.m_terrainHeight = MathUtils::InterpolateClamped(riverHeight, desertHeight, MathUtils::RangeMapClamped(tileGenData.m_humidity, DESERT_HUMIDITY_THRESHOLD, 0.f, 0.f, worldSettings.m_riverToDesertTransitionSpeed));
	}

	return tileGenData;
}



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Destroy()
{
	m_tileIDs.Clear();

	g_renderer->ReleaseVertexBuffer(m_vbo);
	#if defined(_DEBUG)
		g_renderer->ReleaseVertexBuffer(m_debugVBO);
	#endif
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
