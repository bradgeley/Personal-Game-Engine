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
#include "Engine/Math/StatisticsUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "EntityDef.h"
#include "TileDef.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr float SEA_LEVEL						= 0.5f;
constexpr float MOUNTAIN_TERRAIN_HEIGHT			= 0.65f;
constexpr float GRASS_TERRAIN_HEIGHT			= 0.5f;
constexpr float SAND_TERRAIN_HEIGHT				= 0.49f;
constexpr float SHALLOW_WATER_TERRAIN_HEIGHT	= 0.4f;
constexpr float WATER_TERRAIN_HEIGHT			= 0.35f;
constexpr float DEEP_WATER_TERRAIN_HEIGHT		= 0.25f;
constexpr float DESERT_HUMIDITY_THRESHOLD		= 0.1f;



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings, std::vector<SpawnInfo>& out_spawnInfos)
{
	m_chunkCoords = chunkCoords;
	Vec2 chunkOrigin = Vec2(chunkCoords.x, chunkCoords.y) * StaticWorldSettings::s_chunkWidth;
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	m_chunkBounds = AABB2(chunkOrigin, chunkOrigin + tileDims * StaticWorldSettings::s_numTilesInRowF);

	int grassTileDef			= TileDef::GetTileDefID("grass");
	int forestGrassTileDef		= TileDef::GetTileDefID("forestGrass");
	int deepForestGrassTileDef	= TileDef::GetTileDefID("deepForestGrass");
	int wallTileDef				= TileDef::GetTileDefID("wall");
	int riverWaterTileDef		= TileDef::GetTileDefID("riverWater");
	int islandWaterTileDef		= TileDef::GetTileDefID("islandWater");
	int oceanWaterTileDef		= TileDef::GetTileDefID("oceanWater");
	int sandTileDef				= TileDef::GetTileDefID("sand");
	int snowTileDef				= TileDef::GetTileDefID("snow");
	int iceTileDef				= TileDef::GetTileDefID("ice");
	TileDef const* errorTileDef = TileDef::GetTileDef("error");

	// Get VBO ready
	m_vbo = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
	vbo.Initialize<Vertex_PCU>(6 * StaticWorldSettings::s_numTilesInChunk);

	// Get sprite sheet ready
	AssetID terrainID = g_assetManager->LoadSynchronous<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	GridSpriteSheet const* terrainSpriteSheet = g_assetManager->Get<GridSpriteSheet>(terrainID);
	ASSERT_OR_DIE(terrainSpriteSheet != nullptr, "Chunk::Generate - Failed to load terrain sprite sheet");

	m_tileIDs.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), (uint8_t) grassTileDef);
	Vec2 chunkMinsWorldLocation = Vec2(chunkCoords) * StaticWorldSettings::s_chunkWidth;

	// Generate tile IDs
	for (int y = 0; y < StaticWorldSettings::s_numTilesInRow; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x)
		{
			Vec2 worldTileLoc = chunkMinsWorldLocation + (Vec2(x, y) * tileDims);
			TileGeneratedData tileGenData = GenerateTileData(worldTileLoc, worldSettings);; // could cache this off but would increase the memory usage of each chunk by an order of magnitude or so

			int index = m_tileIDs.GetIndexForCoords(x, y);

			if (tileGenData.m_isCold)
			{
				if (tileGenData.m_isOcean || tileGenData.m_isRiver || tileGenData.m_isIsland && tileGenData.m_terrainHeight < SAND_TERRAIN_HEIGHT)
				{
					m_tileIDs.Set(index, (uint8_t) iceTileDef);
				}
				else
				{
					m_tileIDs.Set(index, (uint8_t) snowTileDef);
				}
			}
			else 
			{
				if (tileGenData.m_terrainHeight >= MOUNTAIN_TERRAIN_HEIGHT)
				{
					m_tileIDs.Set(index, (uint8_t) wallTileDef);
				}
				else if (tileGenData.m_terrainHeight >= GRASS_TERRAIN_HEIGHT)
				{
					if (tileGenData.m_isDeepForest)
					{
						m_tileIDs.Set(index, (uint8_t) deepForestGrassTileDef);
					}
					else if (tileGenData.m_isForest)
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
				else if (tileGenData.m_isIsland)
				{
					m_tileIDs.Set(index, (uint8_t) islandWaterTileDef);
				}
				else if (tileGenData.m_isOcean && !tileGenData.m_isDesert)
				{
					m_tileIDs.Set(index, (uint8_t) islandWaterTileDef);
				}
				else if (tileGenData.m_isRiver)
				{
					m_tileIDs.Set(index, (uint8_t) riverWaterTileDef);
				}

				if (tileGenData.m_isDesert && tileGenData.m_terrainHeight >= GRASS_TERRAIN_HEIGHT && tileGenData.m_terrainHeight < MOUNTAIN_TERRAIN_HEIGHT)
				{
					m_tileIDs.Set(index, (uint8_t) sandTileDef);
				}
			}

			// Generate Chunk VBO
			int tileID = (int) m_tileIDs.Get(index);
			TileDef const* tileDef = TileDef::GetTileDef((uint8_t) tileID);
			if (!tileDef)
			{
				tileDef = errorTileDef;
			}

			Rgba8 tint = tileDef->m_tint;
			if (tileID == riverWaterTileDef || tileID == oceanWaterTileDef || tileID == islandWaterTileDef)
			{
				float depth = MathUtils::RangeMapClamped(tileGenData.m_terrainHeight, SEA_LEVEL, 0.f, 0.f, 1.f);
				tint = Rgba8::Lerp(tint, Rgba8::DarkOceanBlue, depth);
			}

			Vec2 mins = chunkOrigin + Vec2(x, y) * StaticWorldSettings::s_tileWidth;
			Vec2 maxs = mins + tileDims;
			AABB2 uvs = terrainSpriteSheet->GetSpriteUVs(tileDef->m_spriteIndex);
			VertexUtils::AddVertsForRect2D(vbo, mins, maxs, tint, uvs, 1.f);

			// Spawn Entities
			if (tileGenData.m_hasTree)
			{
				SpawnInfo spawnInfo;
				spawnInfo.m_def = EntityDef::GetEntityDef("Tree1");
				spawnInfo.m_spawnPos = mins + (tileDims * 0.5f);
				out_spawnInfos.push_back(spawnInfo);
			}
		}
	}

	g_assetManager->Release(terrainID);

	#if defined(_DEBUG)
		m_debugVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
		VertexBuffer& debugVBO = *g_renderer->GetVertexBuffer(m_debugVBO);
		VertexUtils::AddVertsForWireGrid(debugVBO, m_chunkBounds, IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), 0.01f, Rgba8::Black);
		VertexUtils::AddVertsForWireBox2D(debugVBO, m_chunkBounds, 0.03f, Rgba8::Red);
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
TileGeneratedData Chunk::GenerateTileData(Vec2 const& worldTileLocation, WorldSettings const& worldSettings)
{
	TileGeneratedData tileGenData;

	// Terrain Height
	tileGenData.m_terrainHeightOffset = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_terrainHeightOffsetScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
	tileGenData.m_terrainHeightOffset *= tileGenData.m_mountainness;

	// Humidity
	tileGenData.m_humidity = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_humidityScale, 12, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 1));
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
	tileGenData.m_forestness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4));
	tileGenData.m_forestness = MathUtils::SmoothStep3(tileGenData.m_forestness);

	// Riverness
	float rivernessScale = worldSettings.m_rivernessScale;
	tileGenData.m_riverness = GetPerlinNoise2D(worldTileLocation.x, worldTileLocation.y, rivernessScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 5));
	tileGenData.m_riverness = MathUtils::AbsF(tileGenData.m_riverness);

	// River runs through here
	float riverMaxDepth = MathUtils::RangeMapClamped(tileGenData.m_humidity, 1.f, 0.f, worldSettings.m_riverMaxDepth, SEA_LEVEL);
	float riverThreshold = MathUtils::RangeMapClamped(tileGenData.m_humidity, 1.f, 0.f, worldSettings.m_riverThreshold, 0.f);

	// Island
	tileGenData.m_islandness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_islandnessScale, 12, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 6));
	tileGenData.m_islandness = MathUtils::SmoothStep3(tileGenData.m_islandness);

	// Temperature
	tileGenData.m_temperature = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_temperatureScale, 12, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7));
	tileGenData.m_temperature = MathUtils::SmoothStep3(tileGenData.m_temperature);

	// Trees
	float localTreeness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_treeBaseScale, 5, tileGenData.m_forestness, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8));
	tileGenData.m_treeness = localTreeness;

	tileGenData.m_isIsland = tileGenData.m_islandness > worldSettings.m_islandThreshold;
	tileGenData.m_isRiver = tileGenData.m_riverness < riverThreshold;
	tileGenData.m_isOcean = tileGenData.m_oceanness > worldSettings.m_oceanSandThreshold;
	tileGenData.m_isOceanSand = tileGenData.m_oceanness > worldSettings.m_oceanSandThreshold && tileGenData.m_oceanness <= worldSettings.m_oceanShallowWaterThreshold;
	tileGenData.m_isShallowOcean = tileGenData.m_oceanness > worldSettings.m_oceanShallowWaterThreshold && tileGenData.m_oceanness <= worldSettings.m_oceanWaterThreshold;
	tileGenData.m_isMediumOcean = tileGenData.m_oceanness > worldSettings.m_oceanWaterThreshold && tileGenData.m_oceanness <= worldSettings.m_oceanDeepWaterThreshold;
	tileGenData.m_isDeepOcean = tileGenData.m_oceanness > worldSettings.m_oceanDeepWaterThreshold;
	tileGenData.m_isDesert = tileGenData.m_humidity < DESERT_HUMIDITY_THRESHOLD;
	tileGenData.m_isForest = tileGenData.m_forestness > 0.5f;
	tileGenData.m_isDeepForest = tileGenData.m_forestness > 0.75f;
	tileGenData.m_isCold = tileGenData.m_temperature < 0.2f;

	if (tileGenData.m_isIsland)
	{
		tileGenData.m_terrainHeight = SEA_LEVEL + tileGenData.m_terrainHeightOffset - 0.1f;
		if (tileGenData.m_terrainHeight < SEA_LEVEL)
		{
			tileGenData.m_terrainHeight = MathUtils::RangeMapClamped(tileGenData.m_terrainHeight, SEA_LEVEL, SEA_LEVEL - 0.1f, SEA_LEVEL, 0.f);
		}
	}
	else
	{
		if (tileGenData.m_isRiver)
		{
			tileGenData.m_terrainHeightOffset = MathUtils::RangeMapClamped(tileGenData.m_riverness, riverThreshold, 0.f, -0.05f, riverMaxDepth - SEA_LEVEL);
		}

		if (tileGenData.m_isDeepOcean)
		{
			constexpr float deepWaterOffset = DEEP_WATER_TERRAIN_HEIGHT - SEA_LEVEL;
			tileGenData.m_terrainHeightOffset = MathUtils::RangeMapClamped(tileGenData.m_oceanness, worldSettings.m_oceanDeepWaterThreshold, 1.f, deepWaterOffset, -0.5f);
		}
		else if (tileGenData.m_isMediumOcean)
		{
			constexpr float waterOffset = WATER_TERRAIN_HEIGHT - SEA_LEVEL;
			constexpr float deepWaterOffset = DEEP_WATER_TERRAIN_HEIGHT - SEA_LEVEL;
			tileGenData.m_terrainHeightOffset = MathUtils::RangeMapClamped(tileGenData.m_oceanness, worldSettings.m_oceanWaterThreshold, worldSettings.m_oceanDeepWaterThreshold, waterOffset, deepWaterOffset);
		}
		else if (tileGenData.m_isShallowOcean)
		{
			constexpr float waterOffset = WATER_TERRAIN_HEIGHT - SEA_LEVEL;
			constexpr float shallowWaterOffset = SHALLOW_WATER_TERRAIN_HEIGHT - SEA_LEVEL;
			tileGenData.m_terrainHeightOffset = MathUtils::RangeMapClamped(tileGenData.m_oceanness, worldSettings.m_oceanShallowWaterThreshold, worldSettings.m_oceanWaterThreshold, shallowWaterOffset, waterOffset);
		}
		else if (tileGenData.m_isOceanSand)
		{
			constexpr float shallowWaterOffset = SHALLOW_WATER_TERRAIN_HEIGHT - SEA_LEVEL;
			constexpr float seaLevelOffset = 0.f;
			tileGenData.m_terrainHeightOffset = MathUtils::RangeMapClamped(tileGenData.m_oceanness, worldSettings.m_oceanSandThreshold, worldSettings.m_oceanShallowWaterThreshold, seaLevelOffset, shallowWaterOffset);
		}

		tileGenData.m_terrainHeight = SEA_LEVEL + tileGenData.m_terrainHeightOffset;

		if (tileGenData.m_isDesert && tileGenData.m_isOcean)
		{
			tileGenData.m_terrainHeightOffset = MathUtils::AbsF(tileGenData.m_terrainHeightOffset);
			tileGenData.m_terrainHeight = SEA_LEVEL + tileGenData.m_terrainHeightOffset;
		}
		else if (tileGenData.m_isOcean && tileGenData.m_terrainHeight >= WATER_TERRAIN_HEIGHT && tileGenData.m_isRiver)
		{
			// Handle rivers transitioning into ocean
			float riverHeight = SEA_LEVEL + MathUtils::RangeMapClamped(tileGenData.m_riverness, riverThreshold, 0.f, -0.05f, riverMaxDepth - SEA_LEVEL);
			float oceanHeight = tileGenData.m_terrainHeight;
			tileGenData.m_terrainHeight = MathUtils::InterpolateClamped(riverHeight, oceanHeight, MathUtils::RangeMapClamped(tileGenData.m_oceanness, worldSettings.m_oceanShallowWaterThreshold, 1.f, 0.f, worldSettings.m_riverToOceanTransitionSpeed));
		}
		else if (tileGenData.m_isDesert && tileGenData.m_isRiver)
		{
			// Handle rivers transitioning into desert
			float riverHeight = SEA_LEVEL + MathUtils::RangeMapClamped(tileGenData.m_riverness, riverThreshold, 0.f, -0.05f, riverMaxDepth - SEA_LEVEL);
			float desertHeight = 0.55f;
			tileGenData.m_terrainHeight = MathUtils::InterpolateClamped(riverHeight, desertHeight, MathUtils::RangeMapClamped(tileGenData.m_humidity, DESERT_HUMIDITY_THRESHOLD, 0.f, 0.f, worldSettings.m_riverToDesertTransitionSpeed));
		}
	}

	tileGenData.m_canGrowTrees = !tileGenData.m_isOcean && !tileGenData.m_isRiver && tileGenData.m_terrainHeight < MOUNTAIN_TERRAIN_HEIGHT;

	if (tileGenData.m_canGrowTrees)
	{
		float neighborForestness[8] =
		{
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y,	     worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y,	     worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x,		  worldTileLocation.y + 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x,		  worldTileLocation.y - 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y + 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y - 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y + 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y - 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4))),
		};

		float neighborTreeness[8] =
		{
			GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y,		  worldSettings.m_treeBaseScale, 5, neighborForestness[0], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y,		  worldSettings.m_treeBaseScale, 5, neighborForestness[1], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x,	   worldTileLocation.y + 1.f, worldSettings.m_treeBaseScale, 5, neighborForestness[2], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x,	   worldTileLocation.y - 1.f, worldSettings.m_treeBaseScale, 5, neighborForestness[3], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y + 1.f, worldSettings.m_treeBaseScale, 5, neighborForestness[4], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y - 1.f, worldSettings.m_treeBaseScale, 5, neighborForestness[5], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y + 1.f, worldSettings.m_treeBaseScale, 5, neighborForestness[6], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y - 1.f, worldSettings.m_treeBaseScale, 5, neighborForestness[7], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
		};

		// If this tile has the highest treeness of its neighbors and is above a certain threshold, it has a tree
		float* max = StatisticsUtils::GetMax(neighborTreeness, 8);
		tileGenData.m_hasTree = tileGenData.m_treeness > *max;
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
