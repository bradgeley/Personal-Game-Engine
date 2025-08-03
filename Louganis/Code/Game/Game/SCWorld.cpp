// Bradley Christensen - 2024
#include "SCWorld.h"
#include "Chunk.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetActiveChunk(IntVec2 const& coords) const
{
	auto it = m_activeChunks.find(coords);
	if (it != m_activeChunks.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetActiveChunk(int x, int y) const
{
	return GetActiveChunk(IntVec2(x, y));
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetActiveChunk(WorldCoords const& worldCoords) const
{
	return GetActiveChunk(worldCoords.m_chunkCoords.x, worldCoords.m_chunkCoords.y);
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetActiveChunkAtLocation(Vec2 const& worldLocation) const
{
	IntVec2 chunkCoords = GetChunkCoordsAtLocation(worldLocation);
	return GetActiveChunk(chunkCoords);
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetChunkCoordsAtLocation(Vec2 const& worldLocation) const
{
	float chunkWidth = GetChunkWidth();
	Vec2 locationChunkSpace = worldLocation / chunkWidth;
	IntVec2 chunkCoords = IntVec2(locationChunkSpace.GetFloor());
	return chunkCoords;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetGlobalTileCoordsAtLocation(Vec2 const& worldLocation) const
{
	WorldCoords worldCoords = GetWorldCoordsAtLocation(worldLocation);
	return worldCoords.GetGlobalTileCoords(GetNumTilesInRow());
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetLocalTileCoordsAtLocation(Vec2 const& worldLocation) const
{
	IntVec2 chunkCoords = GetChunkCoordsAtLocation(worldLocation);
	Vec2 relativeLocation = worldLocation - (Vec2(chunkCoords.x, chunkCoords.y) * GetChunkWidth());
	Vec2 tileSpaceLocation = relativeLocation / m_worldSettings.m_tileWidth;
	IntVec2 localTileCoords = IntVec2(tileSpaceLocation.GetFloor());
	localTileCoords.x = ClampInt(localTileCoords.x, 0, GetNumTilesInRow() - 1);
	localTileCoords.y = ClampInt(localTileCoords.y, 0, GetNumTilesInRow() - 1);
	return localTileCoords;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetLocalTileCoordsAtLocation(Vec2 const& worldLocation, IntVec2 const& chunkCoords) const
{
	Vec2 relativeLocation = worldLocation - (Vec2(chunkCoords.x, chunkCoords.y) * GetChunkWidth());
	Vec2 tileSpaceLocation = relativeLocation / m_worldSettings.m_tileWidth;
	IntVec2 localTileCoords = IntVec2(tileSpaceLocation.GetFloor());
	localTileCoords.x = ClampInt(localTileCoords.x, 0, GetNumTilesInRow() - 1);
	localTileCoords.y = ClampInt(localTileCoords.y, 0, GetNumTilesInRow() - 1);
	return localTileCoords;
}



//----------------------------------------------------------------------------------------------------------------------
WorldCoords SCWorld::GetWorldCoordsAtOffset(WorldCoords const& worldCoords, IntVec2 const& tileOffset) const
{
	int numTilesPerRow = GetNumTilesInRow();

	WorldCoords result;
	result.m_chunkCoords = worldCoords.m_chunkCoords;
	result.m_localTileCoords = worldCoords.m_localTileCoords + tileOffset;

	// Check north movement
	while (result.m_localTileCoords.y >= numTilesPerRow)
	{
		result.m_localTileCoords.y -= numTilesPerRow;
		result.m_chunkCoords.y++;
	}
	// Check south movement
	while (result.m_localTileCoords.y < 0)
	{
		result.m_localTileCoords.y += numTilesPerRow;
		result.m_chunkCoords.y--;
	}
	// Check east movement
	while (result.m_localTileCoords.x >= numTilesPerRow)
	{
		result.m_localTileCoords.x -= numTilesPerRow;
		result.m_chunkCoords.x++;
	}
	// Check west movement
	while (result.m_localTileCoords.x < 0)
	{
		result.m_localTileCoords.x += numTilesPerRow;
		result.m_chunkCoords.x--;
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
WorldCoords SCWorld::GetWorldCoordsAtLocation(Vec2 const& worldLocation) const
{
	IntVec2 chunkCoords = GetChunkCoordsAtLocation(worldLocation);
	IntVec2 localTileCoords = GetLocalTileCoordsAtLocation(worldLocation, chunkCoords);
	return WorldCoords(chunkCoords, localTileCoords);
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::CalculateChunkBounds(int chunkX, int chunkY) const
{
	int numTilesInRow = GetNumTilesInRow();
	float chunkWidth = m_worldSettings.m_tileWidth * numTilesInRow;
	Vec2 chunkOrigin = Vec2(chunkX, chunkY) * chunkWidth;
	Vec2 tileDims = Vec2(m_worldSettings.m_tileWidth, m_worldSettings.m_tileWidth);
	AABB2 result = AABB2(chunkOrigin, chunkOrigin + tileDims * (float) numTilesInRow);
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBoundsAtWorldPos(Vec2 const& worldPos) const
{
	IntVec2 worldTileCoords = GetGlobalTileCoordsAtLocation(worldPos);
	AABB2 tileBounds;
	tileBounds.mins = Vec2(worldTileCoords) * m_worldSettings.m_tileWidth;
	tileBounds.maxs = tileBounds.mins + Vec2(m_worldSettings.m_tileWidth, m_worldSettings.m_tileWidth);
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBounds(WorldCoords const& worldCoords) const
{
	Vec2 chunkMins = Vec2(worldCoords.m_chunkCoords) * GetChunkWidth();
	
	AABB2 tileBounds;
	tileBounds.mins = chunkMins + Vec2(worldCoords.m_localTileCoords) * m_worldSettings.m_tileWidth;
	tileBounds.maxs = tileBounds.mins + Vec2(m_worldSettings.m_tileWidth, m_worldSettings.m_tileWidth);
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBounds(IntVec2 const& worldTileCoords) const
{
	AABB2 tileBounds;
	tileBounds.mins = Vec2(worldTileCoords) * m_worldSettings.m_tileWidth;
	tileBounds.maxs = tileBounds.mins + Vec2(m_worldSettings.m_tileWidth, m_worldSettings.m_tileWidth);
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetOrCreateActiveChunk(int chunkX, int chunkY)
{
	Chunk* chunk = GetActiveChunk(chunkX, chunkY);
	if (!chunk)
	{
		chunk = new Chunk();
		IntVec2 chunkCoords = IntVec2(chunkX, chunkY);
		chunk->Generate(chunkCoords, m_worldSettings);
		m_activeChunks.emplace(chunkCoords, chunk);
	}
	return chunk;
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::RemoveActiveChunk(IntVec2 const& coords)
{
	auto it = m_activeChunks.find(coords);
	if (it != m_activeChunks.end())
	{
		m_activeChunks.erase(it);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::RemoveActiveChunk(int chunkX, int chunkY)
{
	IntVec2 chunkCoords(chunkX, chunkY);
	RemoveActiveChunk(chunkCoords);
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::RemoveActiveChunks(std::vector<IntVec2> const& coordsList)
{
	for (auto& coords : coordsList)
	{
		RemoveActiveChunk(coords);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ClearActiveChunks()
{
	for (auto chunk : m_activeChunks)
	{
		delete chunk.second;
	}
	m_activeChunks.clear();
}



//----------------------------------------------------------------------------------------------------------------------
int SCWorld::GetNumTilesInRow() const
{
	return Pow(2, m_worldSettings.m_chunkSizePowerOfTwo);
}



//----------------------------------------------------------------------------------------------------------------------
float SCWorld::GetChunkUnloadRadius() const
{
	float chunkLoadRadius = m_worldSettings.m_chunkLoadRadius;
	float chunkUnloadRadius = m_worldSettings.m_chunkUnloadRadius;
	if (chunkUnloadRadius < chunkLoadRadius)
	{
		// Never allow the unload radius to be less than the load radius,
		// or else constant loading/unloading will happen and ruin perf
		chunkUnloadRadius = chunkLoadRadius;
	}
	return chunkUnloadRadius;
}



//----------------------------------------------------------------------------------------------------------------------
float SCWorld::GetChunkWidth() const
{
	return m_worldSettings.m_tileWidth * GetNumTilesInRow();
}
