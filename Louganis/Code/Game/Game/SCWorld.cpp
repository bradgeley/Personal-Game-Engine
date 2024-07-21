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
AABB2 SCWorld::CalculateChunkBounds(int x, int y) const
{
	int numTilesInRow = GetNumTilesInRow();
	float chunkWidth = m_worldSettings.m_tileWidth * numTilesInRow;
	Vec2 chunkOrigin = Vec2(x, y) * chunkWidth;
	Vec2 tileDims = Vec2(m_worldSettings.m_tileWidth, m_worldSettings.m_tileWidth);
	AABB2 result = AABB2(chunkOrigin, chunkOrigin + tileDims * (float) numTilesInRow);
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* SCWorld::GetOrCreateActiveChunk(int x, int y)
{
	Chunk* chunk = GetActiveChunk(x, y);
	if (!chunk)
	{
		chunk = new Chunk();
		IntVec2 chunkCoords = IntVec2(x, y);
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
void SCWorld::RemoveActiveChunk(int x, int y)
{
	IntVec2 chunkCoords(x, y);
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
