// Bradley Christensen - 2022-2025
#include "FlowFieldChunk.h"
#include "Chunk.h"
#include "SCWorld.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/Renderer.h"



constexpr float MAX_DISTANCE = 999.f;



//----------------------------------------------------------------------------------------------------------------------
FlowFieldChunk::FlowFieldChunk(Chunk* chunk, SCWorld* world) :
	m_world(world),
	m_chunk(chunk),
	m_chunkCoords(chunk->m_chunkCoords),
	m_chunkBounds(chunk->m_chunkBounds),
	m_costField(0),
	m_distanceField(MAX_DISTANCE),
	m_gradient(Vec2::ZeroVector),
	m_consideredCells(false)
{
	m_chunk->m_destroyed.SubscribeMethod(this, &FlowFieldChunk::ChunkDestroyed);
	m_debugVBO = g_renderer->MakeVertexBuffer();
}



//----------------------------------------------------------------------------------------------------------------------
FlowFieldChunk::~FlowFieldChunk()
{
	if (m_chunk)
	{
		m_chunk->m_destroyed.UnsubscribeMethod(this, &FlowFieldChunk::ChunkDestroyed);
	}

	g_renderer->ReleaseVertexBuffer(m_debugVBO);
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::HardReset()
{
	m_consideredCells.SetAll(false);
	m_costField.SetAll(0);
	m_distanceField.SetAll(MAX_DISTANCE);
	m_gradient.SetAll(Vec2::ZeroVector);
	g_renderer->GetVertexBuffer(m_debugVBO)->ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::SoftReset()
{
	// Don't need to reset cost field if we're resetting to generate again
	m_consideredCells.SetAll(false);
	m_distanceField.SetAll(MAX_DISTANCE);
	m_gradient.SetAll(Vec2());
	g_renderer->GetVertexBuffer(m_debugVBO)->ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
Chunk* FlowFieldChunk::GetChunk() const
{
	return m_chunk;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 const& FlowFieldChunk::GetChunkBounds() const
{
	return m_chunkBounds;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 FlowFieldChunk::GetChunkCoords() const
{
	return m_chunkCoords;
}



//----------------------------------------------------------------------------------------------------------------------
bool FlowFieldChunk::IsTileSolid(IntVec2 const& localTileCoords) const
{
	if (m_chunk)
	{
		return m_chunk->IsTileSolid(localTileCoords);
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::SetDistanceFieldAt(IntVec2 const& localTileCoords, float distance)
{
	m_distanceField.Set(localTileCoords, distance);
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::GenerateCostField()
{
	if (m_chunk)
	{
		for (int i = 0; i < m_costField.Count(); ++i)
		{
			m_costField.Set(i, m_chunk->GetCost(i));
		}
	}
	else
	{
		for (int i = 0; i < m_costField.Count(); ++i)
		{
			m_costField.Set(i, 1);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::ResetConsideredCells()
{
	m_consideredCells.SetAll(false);
}



//----------------------------------------------------------------------------------------------------------------------
bool FlowFieldChunk::ChunkDestroyed(NamedProperties&)
{
	m_chunk->m_destroyed.UnsubscribeMethod(this, &FlowFieldChunk::ChunkDestroyed);
	m_chunk = nullptr;
	return false;
}
