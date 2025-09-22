// Bradley Christensen - 2022-2025
#include "SCWorld.h"
#include "Chunk.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Core/ErrorUtils.h"
#include <queue>
#include <unordered_set>



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
	Vec2 locationChunkSpace = worldLocation * StaticWorldSettings::s_oneOverChunkWidth;
	IntVec2 chunkCoords = IntVec2(locationChunkSpace.GetFloor());
	return chunkCoords;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetGlobalTileCoordsAtLocation(Vec2 const& worldLocation) const
{
	Vec2 globalTileCoords = worldLocation * StaticWorldSettings::s_oneOverTileWidth;
	return IntVec2(globalTileCoords.GetFloor());
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetLocalTileCoordsAtLocation(Vec2 const& worldLocation) const
{
	IntVec2 chunkCoords = GetChunkCoordsAtLocation(worldLocation);
	Vec2 chunkRelativeLocation = worldLocation - (Vec2(chunkCoords.x, chunkCoords.y) * StaticWorldSettings::s_chunkWidth);
	Vec2 tileSpaceLocation = chunkRelativeLocation * StaticWorldSettings::s_oneOverTileWidth;
	IntVec2 localTileCoords = IntVec2(tileSpaceLocation.GetFloor());
	localTileCoords.x = MathUtils::Clamp(localTileCoords.x, 0, StaticWorldSettings::s_numTilesInRowMinusOne);
	localTileCoords.y = MathUtils::Clamp(localTileCoords.y, 0, StaticWorldSettings::s_numTilesInRowMinusOne);
	return localTileCoords;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetLocalTileCoordsAtLocation(Vec2 const& worldLocation, IntVec2 const& chunkCoords) const
{
	Vec2 chunkMins = Vec2(chunkCoords.x, chunkCoords.y) * StaticWorldSettings::s_chunkWidth;
	Vec2 chunkRelativeLocation = worldLocation - chunkMins;
	Vec2 tileSpaceLocation = chunkRelativeLocation * StaticWorldSettings::s_oneOverTileWidth;
	IntVec2 localTileCoords = IntVec2(tileSpaceLocation.GetFloor());
	localTileCoords.x = MathUtils::Clamp(localTileCoords.x, 0, StaticWorldSettings::s_numTilesInRowMinusOne);
	localTileCoords.y = MathUtils::Clamp(localTileCoords.y, 0, StaticWorldSettings::s_numTilesInRowMinusOne);
	return localTileCoords;
}



//----------------------------------------------------------------------------------------------------------------------
WorldCoords SCWorld::GetWorldCoordsAtOffset(WorldCoords const& worldCoords, IntVec2 const& tileOffset) const
{
	WorldCoords result;
	result.m_chunkCoords = worldCoords.m_chunkCoords;
	result.m_localTileCoords = worldCoords.m_localTileCoords + tileOffset;

	// Check north movement
	while (result.m_localTileCoords.y >= StaticWorldSettings::s_numTilesInRow)
	{
		result.m_localTileCoords.y -= StaticWorldSettings::s_numTilesInRow;
		result.m_chunkCoords.y++;
	}
	// Check south movement
	while (result.m_localTileCoords.y < 0)
	{
		result.m_localTileCoords.y += StaticWorldSettings::s_numTilesInRow;
		result.m_chunkCoords.y--;
	}
	// Check east movement
	while (result.m_localTileCoords.x >= StaticWorldSettings::s_numTilesInRow)
	{
		result.m_localTileCoords.x -= StaticWorldSettings::s_numTilesInRow;
		result.m_chunkCoords.x++;
	}
	// Check west movement
	while (result.m_localTileCoords.x < 0)
	{
		result.m_localTileCoords.x += StaticWorldSettings::s_numTilesInRow;
		result.m_chunkCoords.x--;
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
WorldCoords SCWorld::GetWorldCoordsAtLocation(Vec2 const& worldLocation) const
{
	IntVec2 globalTileCoords = GetGlobalTileCoordsAtLocation(worldLocation);
	WorldCoords result = GetWorldCoordsAtGlobalTileCoords(globalTileCoords);
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
WorldCoords SCWorld::GetWorldCoordsAtGlobalTileCoords(IntVec2 const& globalTileCoords) const
{
	WorldCoords result;
	Vec2 chunkCoordsF = Vec2(globalTileCoords) * StaticWorldSettings::s_oneOverNumTilesInRow;
	result.m_chunkCoords = IntVec2(chunkCoordsF.GetFloor());

	IntVec2 chunkOriginGlobalTileCoords = result.m_chunkCoords * StaticWorldSettings::s_numTilesInRow;
	result.m_localTileCoords = globalTileCoords - chunkOriginGlobalTileCoords;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(WorldCoords const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetCapsuleBounds(start, end, radius);

	ForEachWorldCoordsOverlappingAABB(boundingBox, [&](WorldCoords const& worldCoords) 
	{ 
		AABB2 tileBounds = GetTileBounds(worldCoords);
		if (GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, tileBounds))
		{
			if (!func(worldCoords))
			{
				return false;
			}
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachWorldCoordsOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(WorldCoords const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetDiscBounds(pos, radius);

	ForEachWorldCoordsOverlappingAABB(boundingBox, [&](WorldCoords const& worldCoords)
	{
		AABB2 tileBounds = GetTileBounds(worldCoords);
		if (GeometryUtils::DoesDiscOverlapAABB(pos, radius, tileBounds))
		{
			if (!func(worldCoords))
			{
				return false;
			}
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(WorldCoords const&)>& func) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 startGlobalTileCoords = GetGlobalTileCoordsAtLocation(aabb.mins);
	IntVec2 endGlobalTileCoords = GetGlobalTileCoordsAtLocation(aabb.maxs);

	int startX = MathUtils::Min(startGlobalTileCoords.x, endGlobalTileCoords.x);
	int endX = MathUtils::Max(startGlobalTileCoords.x, endGlobalTileCoords.x);

	int startY = MathUtils::Min(startGlobalTileCoords.y, endGlobalTileCoords.y);
	int endY = MathUtils::Max(startGlobalTileCoords.y, endGlobalTileCoords.y);

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			WorldCoords worldCoords = GetWorldCoordsAtGlobalTileCoords(IntVec2(x, y));
			if (Chunk* chunk = GetActiveChunk(worldCoords))
			{
				if (!func(worldCoords))
				{
					return;
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachSolidWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(WorldCoords const&, Chunk&)>& func) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 startGlobalTileCoords = GetGlobalTileCoordsAtLocation(aabb.mins);
	IntVec2 endGlobalTileCoords = GetGlobalTileCoordsAtLocation(aabb.maxs);

	int startX = MathUtils::Min(startGlobalTileCoords.x, endGlobalTileCoords.x);
	int endX = MathUtils::Max(startGlobalTileCoords.x, endGlobalTileCoords.x);

	int startY = MathUtils::Min(startGlobalTileCoords.y, endGlobalTileCoords.y);
	int endY = MathUtils::Max(startGlobalTileCoords.y, endGlobalTileCoords.y);

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			WorldCoords worldCoords = GetWorldCoordsAtGlobalTileCoords(IntVec2(x, y));
			if (Chunk* chunk = GetActiveChunk(worldCoords))
			{
				if (chunk->IsTileSolid(worldCoords.m_localTileCoords))
				{
					if (!func(worldCoords, *chunk))
					{
						return;
					}
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachSolidWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(WorldCoords const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetCapsuleBounds(start, end, radius);

	ForEachSolidWorldCoordsOverlappingAABB(boundingBox, [&](WorldCoords const& coords, Chunk& chunk)
	{
		AABB2 tileBounds = GetTileBounds(coords);
		if (!GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, tileBounds))
		{
			return true;
		}

		if (chunk.IsTileSolid(coords.m_localTileCoords))
		{
			if (!func(coords))
			{
				return false;
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
void SCWorld::ForEachChunkCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 initialChunkCoords = GetChunkCoordsAtLocation(aabb.mins);
	IntVec2 endChunkCoords = GetChunkCoordsAtLocation(aabb.maxs);

	int startX = MathUtils::Min(initialChunkCoords.x, endChunkCoords.x);
	int endX = MathUtils::Max(initialChunkCoords.x, endChunkCoords.x);

	int startY = MathUtils::Min(initialChunkCoords.y, endChunkCoords.y);
	int endY = MathUtils::Max(initialChunkCoords.y, endChunkCoords.y);

	IntVec2 coords;
	for (coords.x = startX; coords.x <= endX; ++coords.x)
	{
		for (coords.y = startY; coords.y <= endY; ++coords.y)
		{
			if (!func(coords))
			{
				return;
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachChunkCoordsOverlappingCircle_InRadialOrder(Vec2 const& circleCenter, float circleRadius, const std::function<bool(IntVec2 const&)>& func) const
{
	std::queue<IntVec2> openList;
	std::unordered_set<IntVec2> closedList;

	IntVec2 initialChunkCoords = GetChunkCoordsAtLocation(circleCenter);
	openList.push(initialChunkCoords);
	closedList.insert(initialChunkCoords);

	static IntVec2 neighborOffsets[4] = { IntVec2(1, 0), IntVec2(-1, 0), IntVec2(0, 1), IntVec2(0, -1) };
	float radiusSquared = circleRadius * circleRadius;

	ASSERT_OR_DIE(circleRadius < 999'999'999.f && circleRadius >= 0.f, "ForEachChunkCoordsInCircle: Ginormous or negative radius, probably unintended?");

	while (!openList.empty())
	{
		IntVec2 top = openList.front();
		openList.pop();

		if (!func(top))
		{
			return;
		}

		for (IntVec2& neighborOffset : neighborOffsets)
		{
			IntVec2 neighborChunkCoords = top + neighborOffset;
			if (!closedList.insert(neighborChunkCoords).second)
			{
				continue;
			}

			Vec2 neighborChunkCenter = CalculateChunkCenter(neighborChunkCoords.x, neighborChunkCoords.y);
			if (neighborChunkCenter.GetDistanceSquaredTo(circleCenter) < radiusSquared)
			{
				openList.push(neighborChunkCoords);
			}
		}
	}

}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachActiveChunkOverlappingCircle(Vec2 const& circleCenter, float circleRadius, const std::function<bool(Chunk const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetDiscBounds(circleCenter, circleRadius);

	ForEachChunkCoordsOverlappingAABB(boundingBox, [&](IntVec2 const& chunkCoords)
	{
		Chunk* activeChunk = GetActiveChunk(chunkCoords);
		if (activeChunk)
		{
			AABB2 chunkBounds = activeChunk->m_chunkBounds;
			if (GeometryUtils::DoesDiscOverlapAABB(circleCenter, circleRadius, chunkBounds))
			{
				if (!func(*activeChunk))
				{
					return false;
				}
			}
		}
		return true;
	});
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
bool SCWorld::IsPointInsideSolidTile(Vec2 const& worldPos) const
{
	WorldCoords worldCoords = GetWorldCoordsAtLocation(worldPos);
	Chunk* chunk = GetActiveChunk(worldCoords);
	if (chunk)
	{
		return chunk->IsTileSolid(worldCoords.m_localTileCoords);
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::CalculateChunkBounds(int chunkX, int chunkY) const
{
	Vec2 chunkOrigin = Vec2(chunkX, chunkY) * StaticWorldSettings::s_chunkWidth;
	Vec2 chunkDims = Vec2(StaticWorldSettings::s_chunkWidth, StaticWorldSettings::s_chunkWidth);
	AABB2 result = AABB2(chunkOrigin, chunkOrigin + chunkDims);
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 SCWorld::CalculateChunkCenter(int chunkX, int chunkY) const
{
	Vec2 chunkOrigin = Vec2(chunkX, chunkY) * StaticWorldSettings::s_chunkWidth;
	Vec2 chunkHalfDims = Vec2(StaticWorldSettings::s_chunkHalfWidth, StaticWorldSettings::s_chunkHalfWidth);
	Vec2 result = chunkOrigin + chunkHalfDims;
	return result;

}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBoundsAtWorldPos(Vec2 const& worldPos) const
{
	IntVec2 worldTileCoords = GetGlobalTileCoordsAtLocation(worldPos);
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	AABB2 tileBounds;
	tileBounds.mins = Vec2(worldTileCoords) * StaticWorldSettings::s_tileWidth;
	tileBounds.maxs = tileBounds.mins + tileDims;
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBounds(WorldCoords const& worldCoords) const
{
	Vec2 chunkMins = Vec2(worldCoords.m_chunkCoords) * StaticWorldSettings::s_chunkWidth;
	
	AABB2 tileBounds;
	tileBounds.mins = chunkMins + Vec2(worldCoords.m_localTileCoords) * StaticWorldSettings::s_tileWidth;
	tileBounds.maxs = tileBounds.mins + Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBounds(IntVec2 const& worldTileCoords) const
{
	AABB2 tileBounds;
	tileBounds.mins = Vec2(worldTileCoords) * StaticWorldSettings::s_tileWidth;
	tileBounds.maxs = tileBounds.mins + Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::LoadChunk(IntVec2 const& chunkCoords, std::vector<SpawnInfo>& out_entitiesToSpawn)
{
	Chunk* chunk = new Chunk();
	chunk->Generate(chunkCoords, m_worldSettings, out_entitiesToSpawn);
	m_activeChunks.emplace(chunkCoords, chunk);
	return chunk;
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::UnloadChunk(Chunk* chunk)
{
	if (chunk)
	{
		m_activeChunks.erase(chunk->m_chunkCoords);
		chunk->Destroy();
		delete chunk;
		return true;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsChunkLoaded(IntVec2 const& chunkCoords) const
{
	return m_activeChunks.find(chunkCoords) != m_activeChunks.end();
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::RemoveActiveChunk(IntVec2 const& coords)
{
	auto it = m_activeChunks.find(coords);
	if (it != m_activeChunks.end())
	{
		Chunk* chunk = it->second;
		chunk->Destroy();
		delete chunk;
		m_activeChunks.erase(it);
		return true;
	}
	return false;
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
		chunk.second->Destroy();
		delete chunk.second;
	}
	m_activeChunks.clear();
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
bool SCWorld::GetPlayerChangedWorldCoordsThisFrame() const
{
	return m_playerChangedWorldCoordsThisFrame;
}
