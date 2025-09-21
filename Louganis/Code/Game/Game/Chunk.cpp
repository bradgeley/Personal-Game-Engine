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
constexpr float MOUNTAIN_TERRAIN_HEIGHT			= 0.65f;
constexpr float GRASS_TERRAIN_HEIGHT			= 0.5f;
constexpr float SAND_TERRAIN_HEIGHT				= 0.49f;
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
	int forestGrassTileDef = TileDef::GetTileDefID("forestGrass");
	int wallTileDef  = TileDef::GetTileDefID("wall");
	int waterTileDef  = TileDef::GetTileDefID("water");
	int shallowWaterTileDef  = TileDef::GetTileDefID("shallowWater");
	int deepWaterTileDef  = TileDef::GetTileDefID("deepWater");
	int sandTileDef  = TileDef::GetTileDefID("sand");
	TileDef const* errorTileDef = TileDef::GetTileDef("error");

	// m_humidity = Compute2dPerlinNoise(globalX, globalY, 500.f, 6, 0.5f, 2.0f, false, m_seed);
	// m_temperature = Compute2dPerlinNoise(globalX, globalY, 1000.f, 6, 0.5f, 2.0f, false, m_seed + 1);
	// m_treeHeight = GlobalBlockXYGenData::CalculateTreeHeightAtCoords(m_worldXY.x, m_worldXY.y, m_seed + 2);
	// float mountainness = 0.5f + 0.5f * Compute2dPerlinNoise(globalX, globalY, 500.f, 5, 0.5f, 2.0f, false, m_seed + 3);
	// m_mountainness = SmoothStep3(mountainness);
	// float terrainHeightOffset = Compute2dPerlinNoise(globalX, globalY, 250.f, 8, 0.5f, 2.0f, false, m_seed + 4);
	// //mountainness = SmoothStep3( mountainness );
	// terrainHeightOffset = AbsF(terrainHeightOffset) * mountainness;
	// terrainHeightOffset -= 0.1f;
	// terrainHeightOffset *= 50.f;
	// m_terrainHeightZ = SEA_LEVEL + (int) terrainHeightOffset;
	// float soilThicknessPerlin = 1.f - mountainness;
	// m_topSoilThickness = (int) (soilThicknessPerlin * (float) soilMaxThickness);
	// m_caveness = 0.5f + 0.5f * Compute2dPerlinNoise(globalX, globalY, 250.f, 6, 0.5f, 2.f, false, m_seed + 5);

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
			float terrainHeightOffset = GetPerlinNoise2D(worldTileCoords.x, worldTileCoords.y, 100.f, 5, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
			terrainHeightOffset *= 0.5f; // -0.5 to 0.5
			terrainHeightOffset += 0.25f; // -0.25 to 0.75, to bias land over water
			float humidity = GetPerlinNoise2D_01(worldTileCoords.x, worldTileCoords.y, 1000.f, 5, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 1));
			humidity = MathUtils::SmoothStep3(humidity);
			humidity *= -1.f; // Invert so higher moisture is lower value
			humidity = MathUtils::RangeMapClamped(humidity, -1.f, 0.f, 0.f, 0.5f);
			float mountainness = GetPerlinNoise2D(worldTileCoords.x, worldTileCoords.y, 1000.f, 5, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 2));
			mountainness = MathUtils::AbsF(mountainness);
			mountainness = MathUtils::SmoothStep3(mountainness);

			float oceanness = GetPerlinNoise2D_01(worldTileCoords.x, worldTileCoords.y, 2000.f, 5, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 3));
			oceanness = MathUtils::SmoothStep3(oceanness);

			float forestness = GetPerlinNoise2D_01(worldTileCoords.x, worldTileCoords.y, 500.f, 5, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4));
			forestness = MathUtils::SmoothStep3(forestness);

			if (terrainHeightOffset > 0.f)
			{
				terrainHeightOffset *= mountainness;
			}

			if (oceanness > 0.75f)
			{
				terrainHeightOffset = MathUtils::RangeMap(oceanness, 0.75f, 1.f, 0.f, -0.5f);
			}

			float terrainHeight = 0.55f + terrainHeightOffset;

			if (terrainHeight >= MOUNTAIN_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) wallTileDef);
			}
			else if (terrainHeight >= GRASS_TERRAIN_HEIGHT)
			{
				if (forestness > 0.5)
				{
					m_tileIDs.Set(index, (uint8_t) forestGrassTileDef);
				}
				else
				{
					m_tileIDs.Set(index, (uint8_t) grassTileDef);
				}
			}
			else if (terrainHeight >= SAND_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) sandTileDef);
			}
			else if (terrainHeight >= SHALLOW_WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) shallowWaterTileDef);
			}
			else if (terrainHeight >= WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) waterTileDef);
			}
			else // (terrainHeight >= DEEP_WATER_TERRAIN_HEIGHT)
			{
				m_tileIDs.Set(index, (uint8_t) deepWaterTileDef);
			}

			if (humidity < 0.1f && terrainHeight >= GRASS_TERRAIN_HEIGHT)
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
