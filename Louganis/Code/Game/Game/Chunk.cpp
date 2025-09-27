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
constexpr float OBSIDIAN_TERRAIN_HEIGHT			= 0.93f;
constexpr float STONE_TERRAIN_HEIGHT			= 0.7f;
constexpr float CLAY_TERRAIN_HEIGHT				= 0.65f;
constexpr float MOUNTAIN_TERRAIN_HEIGHT			= 0.65f;
//----------------------------------------------------------------------------------------------------------------------
constexpr float GRASS_TERRAIN_HEIGHT			= 0.5f;
constexpr float SEA_LEVEL						= 0.5f;
//----------------------------------------------------------------------------------------------------------------------
constexpr float SAND_TERRAIN_HEIGHT				= 0.49f;
constexpr float SHALLOW_WATER_TERRAIN_HEIGHT	= 0.4f;
constexpr float WATER_TERRAIN_HEIGHT			= 0.35f;
constexpr float DEEP_WATER_TERRAIN_HEIGHT		= 0.25f;
//----------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings, std::vector<SpawnInfo>& out_spawnInfos)
{
	m_chunkCoords = chunkCoords;
	Vec2 chunkOrigin = Vec2(chunkCoords.x, chunkCoords.y) * StaticWorldSettings::s_chunkWidth;
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	m_chunkBounds = AABB2(chunkOrigin, chunkOrigin + tileDims * StaticWorldSettings::s_numTilesInRowF);

	Tile grassTile				= TileDef::GetDefaultTile("grass");
	Tile forestGrassTile		= TileDef::GetDefaultTile("forestGrass");
	Tile deepForestGrassTile	= TileDef::GetDefaultTile("deepForestGrass");
	Tile stoneTile				= TileDef::GetDefaultTile("stone");
	Tile clayTile				= TileDef::GetDefaultTile("clay");
	Tile obsidianTile			= TileDef::GetDefaultTile("obsidian");
	Tile waterTile				= TileDef::GetDefaultTile("islandWater");
	Tile sandTile				= TileDef::GetDefaultTile("sand");
	Tile snowTile				= TileDef::GetDefaultTile("snow");
	Tile iceTile				= TileDef::GetDefaultTile("ice");
	Tile errorTile				= TileDef::GetDefaultTile("error");

	Tile& defaultTile = grassTile;
	m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), defaultTile);

	// Generate tile IDs
	for (int y = 0; y < StaticWorldSettings::s_numTilesInRow; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x)
		{
			WorldCoords tileWorldCoords = WorldCoords(m_chunkCoords, IntVec2(x, y));
			TileGeneratedData tileGenData = GenerateTileData(tileWorldCoords.GetGlobalTileCoords(), worldSettings); // could cache this off but would increase the memory usage of each chunk by an order of magnitude or more
			Vec2 tileOrigin = chunkOrigin + Vec2(x, y) * StaticWorldSettings::s_tileWidth;

			int index = m_tiles.GetIndexForCoords(x, y);

			if (tileGenData.m_terrainHeight >= OBSIDIAN_TERRAIN_HEIGHT)
			{
				m_tiles.Set(index, obsidianTile);
			}
			else if (tileGenData.m_terrainHeight >= STONE_TERRAIN_HEIGHT)
			{
				m_tiles.Set(index, stoneTile);
			}
			else if (tileGenData.m_terrainHeight >= CLAY_TERRAIN_HEIGHT)
			{
				m_tiles.Set(index, clayTile);
			}
			else if (tileGenData.m_terrainHeight >= GRASS_TERRAIN_HEIGHT)
			{
				if (tileGenData.m_isDeepForest)
				{
					m_tiles.Set(index, deepForestGrassTile);
				}
				else if (tileGenData.m_isForest)
				{
					m_tiles.Set(index, forestGrassTile);
				}
				else
				{
					m_tiles.Set(index, grassTile);
				}
			}
			else if (tileGenData.m_terrainHeight >= SAND_TERRAIN_HEIGHT)
			{
				m_tiles.Set(index, sandTile);
			}
			else
			{
				m_tiles.Set(index, waterTile);
			}

			if (tileGenData.m_isDesert && tileGenData.m_terrainHeight >= GRASS_TERRAIN_HEIGHT && tileGenData.m_terrainHeight < MOUNTAIN_TERRAIN_HEIGHT)
			{
				m_tiles.Set(index, sandTile);
			}
			
			// Handle ice and snow for now hackily
			if (tileGenData.m_isCold)
			{
				if (tileGenData.m_terrainHeight < SEA_LEVEL)
				{
					m_tiles.Set(index, iceTile);
				}
				else if (tileGenData.m_terrainHeight < MOUNTAIN_TERRAIN_HEIGHT)
				{
					m_tiles.Set(index, snowTile);
				}
			}

			Tile& tile = m_tiles.GetRef(index);
			tile.m_staticLighting = static_cast<uint8_t>(tileGenData.m_staticLighting01 * 255.f);

			// Spawn Entities
			if (tileGenData.m_treeDef != nullptr)
			{
				SpawnInfo spawnInfo;
				spawnInfo.m_def = tileGenData.m_treeDef;
				spawnInfo.m_spawnPos = tileOrigin + (tileDims * 0.5f);
				spawnInfo.m_spawnPos.y += tileGenData.m_treeScale;
				spawnInfo.m_spawnScale = tileGenData.m_treeScale;
				spawnInfo.m_spawnTint = tileGenData.m_treeTint;
				out_spawnInfos.push_back(spawnInfo);
			}
		}
	}

	#if defined(_DEBUG)
		m_debugVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
		VertexBuffer& debugVBO = *g_renderer->GetVertexBuffer(m_debugVBO);
		VertexUtils::AddVertsForWireGrid(debugVBO, m_chunkBounds, IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), 0.01f, Rgba8::Black);
		VertexUtils::AddVertsForWireBox2D(debugVBO, m_chunkBounds, 0.03f, Rgba8::Red);
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Chunk::GenerateVBO()
{
	if (m_numDirtyTiles == 0)
	{
		return;
	}

	Vec2 chunkOrigin = Vec2(m_chunkCoords.x, m_chunkCoords.y) * StaticWorldSettings::s_chunkWidth;
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);

	// Get sprite sheet ready
	AssetID terrainID = g_assetManager->LoadSynchronous<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	GridSpriteSheet const* terrainSpriteSheet = g_assetManager->Get<GridSpriteSheet>(terrainID);
	ASSERT_OR_DIE(terrainSpriteSheet != nullptr, "Chunk::Generate - Failed to load terrain sprite sheet");

	// Get VBO ready
	if (m_vbo == RendererUtils::InvalidID)
	{
		m_vbo = g_renderer->MakeVertexBuffer<Vertex_PCU>(StaticWorldSettings::s_numVertsInChunk);
	}

	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
	vbo.Resize(StaticWorldSettings::s_numVertsInChunk);

	for (int y = 0; y < StaticWorldSettings::s_numTilesInRow; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x)
		{
			int index = m_tiles.GetIndexForCoords(x, y);
			if (m_tiles.Get(index).IsVBODirty())
			{
				Tile& tile = m_tiles.GetRef(index);
				TileDef const* tileDef = TileDef::GetTileDef((uint8_t) tile.m_id);
				ASSERT_OR_DIE(tileDef != nullptr, "Chunk::Generate - Failed to find TileDef for tile id.");

				Vec2 mins = chunkOrigin + Vec2(x, y) * StaticWorldSettings::s_tileWidth;
				Vec2 maxs = mins + tileDims;
				AABB2 uvs = terrainSpriteSheet->GetSpriteUVs(tileDef->m_spriteIndex);

				int firstVertIndex = index * 6;
				Vertex_PCU& firstVert = vbo.GetVert<Vertex_PCU>(firstVertIndex);
				float staticLighting01 = static_cast<float>(tile.m_staticLighting) / 255.f;
				Rgba8 tint = tileDef->m_tint * staticLighting01;
				VertexUtils::WriteVertsForRect2D(&firstVert, mins, maxs, tint, uvs, 1.f);
				SetTileDirty(tile, false);
			}
		}
	}

	g_assetManager->Release(terrainID);
}



//----------------------------------------------------------------------------------------------------------------------
TileGeneratedData Chunk::GenerateTileData(IntVec2 const& globalTileCoords, WorldSettings const& worldSettings)
{
	TileGeneratedData tileGenData;

	Vec2 worldTileLocation = Vec2(globalTileCoords);

	// Mountainness
	tileGenData.m_mountainness = GetPerlinNoise2D(worldTileLocation.x, worldTileLocation.y, worldSettings.m_mountainnessScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
	tileGenData.m_mountainness = MathUtils::AbsF(tileGenData.m_mountainness);
	tileGenData.m_mountainness = MathUtils::SmoothStep3(tileGenData.m_mountainness);

	// Terrain Height
	tileGenData.m_terrainHeightOffset = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_terrainHeightOffsetScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 1));
	tileGenData.m_terrainHeightOffset *= tileGenData.m_mountainness;

	// Humidity
	tileGenData.m_humidity = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_humidityScale, 12, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 2));
	tileGenData.m_humidity = MathUtils::SmoothStep3(tileGenData.m_humidity);
	tileGenData.m_humidity *= -1.f; // Invert so higher moisture is lower value
	tileGenData.m_humidity = MathUtils::RangeMapClamped(tileGenData.m_humidity, -1.f, 0.f, 0.f, 0.5f);

	// Oceanness
	tileGenData.m_oceanness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_oceannessScale, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 3));
	tileGenData.m_oceanness = MathUtils::SmoothStep3(tileGenData.m_oceanness);

	// Riverness
	float rivernessScale = worldSettings.m_rivernessScale;
	tileGenData.m_riverness = GetPerlinNoise2D(worldTileLocation.x, worldTileLocation.y, rivernessScale, 12, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 4));
	tileGenData.m_riverness = MathUtils::AbsF(tileGenData.m_riverness);

	// River runs through here
	float riverMaxDepth = MathUtils::RangeMapClamped(tileGenData.m_humidity, 1.f, 0.f, worldSettings.m_riverMaxDepth, SEA_LEVEL);
	float riverThreshold = MathUtils::RangeMapClamped(tileGenData.m_humidity, 1.f, 0.f, worldSettings.m_riverThreshold, 0.f);

	// Island
	tileGenData.m_islandness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_islandnessScale, 12, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 5));
	tileGenData.m_islandness = MathUtils::SmoothStep3(tileGenData.m_islandness);

	// Temperature
	tileGenData.m_temperature = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_temperatureScale, 12, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 6));
	tileGenData.m_temperature = MathUtils::SmoothStep3(tileGenData.m_temperature);

	// Forestness
	tileGenData.m_forestness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7));
	tileGenData.m_forestness = MathUtils::SmoothStep3(tileGenData.m_forestness);

	tileGenData.m_isIsland = tileGenData.m_islandness > worldSettings.m_islandThreshold;
	tileGenData.m_isRiver = tileGenData.m_riverness < riverThreshold;
	tileGenData.m_isOcean = tileGenData.m_oceanness > worldSettings.m_oceanSandThreshold;
	tileGenData.m_isOceanSand = tileGenData.m_oceanness > worldSettings.m_oceanSandThreshold && tileGenData.m_oceanness <= worldSettings.m_oceanShallowWaterThreshold;
	tileGenData.m_isShallowOcean = tileGenData.m_oceanness > worldSettings.m_oceanShallowWaterThreshold && tileGenData.m_oceanness <= worldSettings.m_oceanWaterThreshold;
	tileGenData.m_isMediumOcean = tileGenData.m_oceanness > worldSettings.m_oceanWaterThreshold && tileGenData.m_oceanness <= worldSettings.m_oceanDeepWaterThreshold;
	tileGenData.m_isDeepOcean = tileGenData.m_oceanness > worldSettings.m_oceanDeepWaterThreshold;
	tileGenData.m_isDesert = tileGenData.m_humidity < worldSettings.m_desertHumidityThreshold;
	tileGenData.m_isForest = tileGenData.m_forestness > worldSettings.m_forestThreshold;
	tileGenData.m_isDeepForest = tileGenData.m_forestness > worldSettings.m_deepForestThreshold;
	tileGenData.m_isCold = tileGenData.m_temperature < worldSettings.m_coldThreshold;


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

	float terrainHeight = SEA_LEVEL + tileGenData.m_terrainHeightOffset;
	tileGenData.m_terrainHeight = terrainHeight;

	if (tileGenData.m_isOcean && tileGenData.m_isRiver)
	{
		// Handle rivers transitioning into ocean
		float riverHeight = SEA_LEVEL + MathUtils::RangeMapClamped(tileGenData.m_riverness, riverThreshold, 0.f, -0.05f, riverMaxDepth - SEA_LEVEL);
		float oceanHeight = tileGenData.m_terrainHeight;
		if (oceanHeight < SEA_LEVEL && oceanHeight >= SAND_TERRAIN_HEIGHT)
		{
			tileGenData.m_terrainHeight = riverHeight; //  MathUtils::InterpolateClamped(riverHeight, oceanHeight, MathUtils::RangeMapClamped(tileGenData.m_oceanness, worldSettings.m_oceanShallowWaterThreshold, 1.f, 0.f, worldSettings.m_riverToOceanTransitionSpeed));
		}
		else
		{
			tileGenData.m_terrainHeight = oceanHeight;
		}
	}

	tileGenData.m_terrainHeight = MathUtils::Clamp01F(tileGenData.m_terrainHeight);

	// Trees
	float treeScale = worldSettings.m_treeBaseScale;
	if (tileGenData.m_isDesert)
	{
		treeScale /= worldSettings.m_desertTreeMultiplier; // make scale larger when multiplier is < 1
	}
	float localTreeness = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, treeScale, 5, tileGenData.m_forestness, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8));
	tileGenData.m_treeness = localTreeness;

	tileGenData.m_canGrowTrees = tileGenData.m_terrainHeight >= SEA_LEVEL && tileGenData.m_terrainHeight < MOUNTAIN_TERRAIN_HEIGHT;

	if (tileGenData.m_canGrowTrees)
	{
		float neighborForestness[8] =
		{
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y,	     worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y,	     worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x,		  worldTileLocation.y + 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x,		  worldTileLocation.y - 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y + 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y - 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y + 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
			MathUtils::SmoothStep3(GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y - 1.f, worldSettings.m_forestnessScale, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 7))),
		};

		float neighborTreeness[8] =
		{
			GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y,		  treeScale, 5, neighborForestness[0], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y,		  treeScale, 5, neighborForestness[1], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x,	   worldTileLocation.y + 1.f, treeScale, 5, neighborForestness[2], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x,	   worldTileLocation.y - 1.f, treeScale, 5, neighborForestness[3], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y + 1.f, treeScale, 5, neighborForestness[4], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x + 1.f, worldTileLocation.y - 1.f, treeScale, 5, neighborForestness[5], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y + 1.f, treeScale, 5, neighborForestness[6], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
			GetPerlinNoise2D_01(worldTileLocation.x - 1.f, worldTileLocation.y - 1.f, treeScale, 5, neighborForestness[7], 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed + 8)),
		};

		// If this tile has the highest treeness of its neighbors and is above a certain threshold, it has a tree
		float* max = StatisticsUtils::GetMax(neighborTreeness, 8);
		bool hasTree = tileGenData.m_treeness > *max;
		if (hasTree)
		{
			if (tileGenData.m_isCold)
			{
				tileGenData.m_treeDef = EntityDef::GetEntityDef("DeadTree1");
			}
			else if (tileGenData.m_isDesert)
			{
				tileGenData.m_treeDef = EntityDef::GetEntityDef("Cactus1");
			}
			else
			{
				tileGenData.m_treeDef = EntityDef::GetEntityDef("Tree1");
			}
		}
		float averageTreeHeight = 2.f + tileGenData.m_forestness * 2.f;
		float treeHeightOffset = GetPerlinNoise2D(worldTileLocation.x, worldTileLocation.y, tileGenData.m_forestness, 1, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
		tileGenData.m_treeScale = averageTreeHeight + treeHeightOffset;
		float t = GetPerlinNoise2D_01(worldTileLocation.x, worldTileLocation.y, 25.f, 2, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
		t = t * 0.5f;
		tileGenData.m_treeTint = Rgba8::Lerp(Rgba8::White, Rgba8::ForestGreen, t);
	}

	// Static lighting
	if (tileGenData.m_terrainHeight < SEA_LEVEL)
	{
		// Get brighter as we go from deep water to sea level
		tileGenData.m_staticLighting01 = MathUtils::RangeMapClamped(tileGenData.m_terrainHeight, 0.f, SEA_LEVEL, 0.f, 1.f);
	}
	else if (tileGenData.m_terrainHeight >= MOUNTAIN_TERRAIN_HEIGHT)
	{
		tileGenData.m_staticLighting01 = MathUtils::RangeMapClamped(tileGenData.m_terrainHeight, MOUNTAIN_TERRAIN_HEIGHT, 1.f, 1.f, 0.25f);
	}
	else
	{
		tileGenData.m_staticLighting01 = MathUtils::RangeMapClamped(tileGenData.m_terrainHeight, SEA_LEVEL, MOUNTAIN_TERRAIN_HEIGHT, 1.f, 0.75f);
	}


	return tileGenData;
}



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Destroy()
{
	m_tiles.Clear();

	g_renderer->ReleaseVertexBuffer(m_vbo);
	#if defined(_DEBUG)
		g_renderer->ReleaseVertexBuffer(m_debugVBO);
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
bool Chunk::IsTileSolid(IntVec2 const& localTileCoords) const
{
	Tile tile = m_tiles.Get(localTileCoords);
	return tile.IsSolid();
}



//----------------------------------------------------------------------------------------------------------------------
bool Chunk::IsTileSolid(int localTileIndex) const
{
	Tile tile = m_tiles.Get(localTileIndex);
	return tile.IsSolid();
}



//----------------------------------------------------------------------------------------------------------------------
uint8_t Chunk::GetCost(int localTileIndex) const
{
	Tile tile = m_tiles.Get(localTileIndex);
	TileDef const* tileDef = TileDef::GetTileDef(tile.m_id);
	if (tileDef)
	{
		return tileDef->m_cost;
	}
	return (uint8_t)255;
}



//----------------------------------------------------------------------------------------------------------------------
void Chunk::SetTileDirty(int localTileIndex, bool isDirty)
{
	Tile tile = m_tiles.Get(localTileIndex);
	SetTileDirty(tile, isDirty);
}



//----------------------------------------------------------------------------------------------------------------------
void Chunk::SetTileDirty(Tile& tile, bool isDirty)
{
	tile.SetDirty(isDirty);
	m_numDirtyTiles += isDirty ? 1 : -1;
}
