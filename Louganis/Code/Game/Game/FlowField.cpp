// Bradley Christensen - 2025
#include "FlowField.h"
#include "FlowFieldChunk.h"



//----------------------------------------------------------------------------------------------------------------------
FlowFieldChunk* FlowField::GetActiveChunk(IntVec2 const& chunkCoords)
{
	auto it = m_activeFlowFieldChunks.find(chunkCoords);
	if (it != m_activeFlowFieldChunks.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool FlowField::Seed(WorldCoords const& worldCoords)
{
	FlowFieldChunk* chunk = GetActiveChunk(worldCoords.m_chunkCoords);
	if (chunk)
	{
		chunk->SetDistanceFieldAt(worldCoords.m_localTileCoords, 0.f);
	}
	return chunk != nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void FlowField::HardReset()
{
	for (auto& it : m_activeFlowFieldChunks)
	{
		FlowFieldChunk* chunk = it.second;
		chunk->HardReset();
		delete chunk;
	}

	m_activeFlowFieldChunks.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void FlowField::SoftReset()
{
	for (auto& it : m_activeFlowFieldChunks)
	{
		FlowFieldChunk* chunk = it.second;
		chunk->SoftReset();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlowField::ResetConsideredCells()
{
	for (auto& it : m_activeFlowFieldChunks)
	{
		FlowFieldChunk* chunk = it.second;
		chunk->ResetConsideredCells();
	}
}
