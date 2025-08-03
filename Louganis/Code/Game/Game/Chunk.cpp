// Bradley Christensen - 2024
#include "Chunk.h"
#include "WorldSettings.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"
#include "TileDef.h"



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings)
{
	m_chunkCoords = chunkCoords;
	int numTilesInRow = Pow(2, worldSettings.m_chunkSizePowerOfTwo);
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
			float wallNoise = GetPerlinNoise2D(worldTileCoords.x, worldTileCoords.y, 100.f, 8, 0.5f, 2.f, true, worldSettings.m_worldSeed);
			if (wallNoise > 0.f)
			{
				m_tileIDs.Set(index, (uint8_t) wallTileDef);
			}
		}
	}

	// Generate Vbo
	m_vbo.Initialize(6 * numTilesInChunk);
	auto& verts = m_vbo.GetMutableVerts();
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
			AddVertsForRect2D(verts, mins, maxs, tint);
		}
	}

#if defined(_DEBUG)
	auto& debugVerts = m_debugVBO.GetMutableVerts();
	AddVertsForWireGrid(debugVerts, m_chunkBounds, IntVec2(numTilesInRow, numTilesInRow), 0.01f, Rgba8::Black);
	AddVertsForWireBox2D(debugVerts, m_chunkBounds, 0.03f, Rgba8::Red);
#endif
}



//----------------------------------------------------------------------------------------------------------------------
void Chunk::Destroy()
{
	m_tileIDs.Clear();
	m_vbo.ClearVerts();
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
bool Chunk::IsTileSolid(int tileIndex) const
{
	uint8_t tileID = m_tileIDs.Get(tileIndex);
	TileDef const* tileDef = TileDef::GetTileDef(tileID);
	if (tileDef)
	{
		return tileDef->IsSolid();
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
uint8_t Chunk::GetCost(int tileIndex) const
{
	uint8_t tileID = m_tileIDs.Get(tileIndex);
	TileDef const* tileDef = TileDef::GetTileDef(tileID);
	if (tileDef)
	{
		return tileDef->m_cost;
	}
	return (uint8_t)255;
}
