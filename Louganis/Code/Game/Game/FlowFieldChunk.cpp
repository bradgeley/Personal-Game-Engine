// Bradley Christensen - 2024
#include "FlowFieldChunk.h"
#include "Chunk.h"

constexpr float MAX_DISTANCE = 999.f;

//----------------------------------------------------------------------------------------------------------------------
FlowFieldChunk::FlowFieldChunk(Chunk* chunk) :
	m_chunk(chunk),
	m_costField(0),
	m_distanceField(MAX_DISTANCE),
	m_gradient(Vec2::ZeroVector),
	m_consideredCells(false)
{
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::HardReset()
{
	m_consideredCells.SetAll(false);
	m_costField.SetAll(0);
	m_distanceField.SetAll(MAX_DISTANCE);
	m_gradient.SetAll(Vec2::ZeroVector);
	m_debugVBO.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::SoftReset()
{
	// Don't need to reset cost field if we're resetting to generate again
	m_consideredCells.SetAll(false);
	m_distanceField.SetAll(MAX_DISTANCE);
	m_gradient.SetAll(Vec2());
	m_debugVBO.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 FlowFieldChunk::GetChunkCoords() const
{
	if (m_chunk)
	{
		return m_chunk->m_chunkCoords;
	}
	return IntVec2();
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::SetDistanceFieldAt(IntVec2 const& localTileCoords, float distance)
{
	m_distanceField.Set(localTileCoords, distance);
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::GenerateCostField()
{
	for (int i = 0; i < m_costField.Count(); ++i)
	{
		m_costField.Set(i, m_chunk->GetCost(i));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlowFieldChunk::ResetConsideredCells()
{
	m_consideredCells.SetAll(false);
}
