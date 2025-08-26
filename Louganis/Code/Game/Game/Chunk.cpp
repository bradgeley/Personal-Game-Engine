// Bradley Christensen - 2022-2025
#include "Chunk.h"
#include "WorldSettings.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "TileDef.h"



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings)
{
	m_chunkCoords = chunkCoords;
	int numTilesInRow = MathUtils::Pow(2, s_worldChunkSizePowerOfTwo);
	int numTilesInChunk = numTilesInRow * numTilesInRow;
	float chunkWidth = worldSettings.m_tileWidth * numTilesInRow;
	Vec2 chunkOrigin = Vec2(chunkCoords.x, chunkCoords.y) * chunkWidth;
	Vec2 tileDims = Vec2(worldSettings.m_tileWidth, worldSettings.m_tileWidth);
	m_chunkBounds = AABB2(chunkOrigin, chunkOrigin + tileDims * (float) numTilesInRow);

	int grassTileDef = TileDef::GetTileDefID("grass");
	int wallTileDef  = TileDef::GetTileDefID("wall");
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
			float wallNoise = GetPerlinNoise2D(worldTileCoords.x, worldTileCoords.y, 100.f, 8, 0.5f, 2.f, true, static_cast<unsigned int>(worldSettings.m_worldSeed));
			if (wallNoise > 0.f)
			{
				m_tileIDs.Set(index, (uint8_t) wallTileDef);
			}
		}
	}

	// Generate Vbo
	m_vbo = g_renderer->MakeVertexBuffer();
	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);

	vbo.Initialize(6 * numTilesInChunk);
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
			Vec2 mins = chunkOrigin + Vec2(x, y) * worldSettings.m_tileWidth;
			Vec2 maxs = mins + tileDims;
			AddVertsForRect2D(vbo, mins, maxs, tint);
		}
	}

#if defined(_DEBUG)
	m_debugVBO = g_renderer->MakeVertexBuffer();
	VertexBuffer& debugVBO = *g_renderer->GetVertexBuffer(m_debugVBO);
	AddVertsForWireGrid(debugVBO, m_chunkBounds, IntVec2(numTilesInRow, numTilesInRow), 0.01f, Rgba8::Black);
	AddVertsForWireBox2D(debugVBO, m_chunkBounds, 0.03f, Rgba8::Red);
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
