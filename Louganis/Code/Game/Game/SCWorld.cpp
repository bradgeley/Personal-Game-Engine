// Bradley Christensen - 2024
#include "SCWorld.h"
#include "Chunk.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
IntVec2 s_neighborOffsets[8] =
{
	IntVec2(1, 0),
	IntVec2(-1, 0),
	IntVec2(0, 1),
	IntVec2(0, -1),
	IntVec2(1, 1),
	IntVec2(-1, -1),
	IntVec2(-1, 1),
	IntVec2(1, -1)
};



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetActiveChunk(IntVec2 const& chunkCoords) const
{
	auto it = m_activeChunks.find(chunkCoords);
	if (it != m_activeChunks.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetActiveChunk(int chunkX, int chunkY) const
{
	return GetActiveChunk(IntVec2(chunkX, chunkY));
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
	localTileCoords.x = MathUtils::ClampInt(localTileCoords.x, 0, GetNumTilesInRow() - 1);
	localTileCoords.y = MathUtils::ClampInt(localTileCoords.y, 0, GetNumTilesInRow() - 1);
	return localTileCoords;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetLocalTileCoordsAtLocation(Vec2 const& worldLocation, IntVec2 const& chunkCoords) const
{
	Vec2 relativeLocation = worldLocation - (Vec2(chunkCoords.x, chunkCoords.y) * GetChunkWidth());
	Vec2 tileSpaceLocation = relativeLocation / m_worldSettings.m_tileWidth;
	IntVec2 localTileCoords = IntVec2(tileSpaceLocation.GetFloor());
	localTileCoords.x = MathUtils::ClampInt(localTileCoords.x, 0, GetNumTilesInRow() - 1);
	localTileCoords.y = MathUtils::ClampInt(localTileCoords.y, 0, GetNumTilesInRow() - 1);
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
void SCWorld::GetEightNeighborWorldCoords(WorldCoords const& worldCoords, WorldCoords* eightNeighborsArray) const
{
	for (int i = 0; i < 8; ++i)
	{
		eightNeighborsArray[i] = GetWorldCoordsAtOffset(worldCoords, s_neighborOffsets[i]);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(WorldCoords&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetCapsuleBounds(start, end, radius);
	int tilesInChunk = GetNumTilesInChunk();

	ForEachChunkOverlappingAABB(boundingBox, [&](Chunk& chunk) 
	{ 
		if (!GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, chunk.m_chunkBounds))
		{
			return true;
		}

		WorldCoords worldCoords;
		worldCoords.m_chunkCoords = chunk.m_chunkCoords;
		for (int i = 0; i < tilesInChunk; ++i)
		{
			worldCoords.m_localTileCoords = chunk.m_tileIDs.GetCoordsForIndex(i);
			AABB2 tileBounds = GetTileBounds(worldCoords);
			if (GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, tileBounds))
			{
				if (!func(worldCoords))
				{
					return false;
				}
			}
		}
		return true; 
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachChunkOverlappingAABB(AABB2 const& aabb, const std::function<bool(Chunk&)>& func) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 initialChunkCoords = GetChunkCoordsAtLocation(aabb.mins);
	IntVec2 endChunkCoords = GetChunkCoordsAtLocation(aabb.maxs);

	int startX = MathUtils::Min(initialChunkCoords.x, endChunkCoords.x);
	int endX = MathUtils::Max(initialChunkCoords.x, endChunkCoords.x);

	int startY = MathUtils::Min(initialChunkCoords.y, endChunkCoords.y);
	int endY = MathUtils::Max(initialChunkCoords.y, endChunkCoords.y);

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			if (Chunk* chunk = GetActiveChunk(x, y))
			{
				if (!func(*chunk))
				{
					return;
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::GetWorldCoordsOverlappingCapsule(std::vector<WorldCoords>& out_worldCoords, Vec2 const& start, Vec2 const& end, float radius) const
{
	std::vector<Chunk*> chunks;
	AABB2 boundingBox = GeometryUtils::GetCapsuleBounds(start, end, radius);
	GetChunksOverlappingAABB(chunks, boundingBox);

	// Conservative estimate
	out_worldCoords.reserve((GetNumTilesInRow()) * chunks.size());

	int tilesInChunk = GetNumTilesInChunk();

	for (Chunk* chunk : chunks)
	{
		if (!GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, chunk->m_chunkBounds))
		{
			continue;
		}

		WorldCoords worldCoords;
		worldCoords.m_chunkCoords = chunk->m_chunkCoords;
		for (int i = 0; i < tilesInChunk; ++i)
		{
			worldCoords.m_localTileCoords = chunk->m_tileIDs.GetCoordsForIndex(i);
			AABB2 tileBounds = GetTileBounds(worldCoords);
			if (GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, tileBounds))
			{
				out_worldCoords.push_back(worldCoords);
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::GetChunksOverlappingAABB(std::vector<Chunk*>& out_chunks, AABB2 const& aabb) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 initialChunkCoords = GetChunkCoordsAtLocation(aabb.mins);
	IntVec2 endChunkCoords = GetChunkCoordsAtLocation(aabb.maxs);

	int startX = MathUtils::Min(initialChunkCoords.x, endChunkCoords.x);
	int endX = MathUtils::Max(initialChunkCoords.x, endChunkCoords.x);

	int startY = MathUtils::Min(initialChunkCoords.y, endChunkCoords.y);
	int endY = MathUtils::Max(initialChunkCoords.y, endChunkCoords.y);

	size_t numToReserve = (size_t) MathUtils::Abs(endY - startY) * MathUtils::Abs(endX - startX);
	out_chunks.reserve(numToReserve);

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			if (Chunk* chunk = GetActiveChunk(x, y))
			{
				out_chunks.push_back(chunk);
			}
		}
	}
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
		Chunk* chunk = it->second;
		chunk->Destroy();
		delete chunk;
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
	for (IntVec2 const& coords : coordsList)
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
	return MathUtils::Pow(2, s_worldChunkSizePowerOfTwo);
}



//----------------------------------------------------------------------------------------------------------------------
int SCWorld::GetNumTilesInChunk() const
{
	int numInRow = GetNumTilesInRow();
	return numInRow * numInRow;
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
