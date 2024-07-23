// Bradley Christensen - 2024
#include "FlowFieldChunk.h"
#include "Chunk.h"



//----------------------------------------------------------------------------------------------------------------------
FlowFieldChunk::FlowFieldChunk(Chunk* correspondingChunk) : 
	m_chunk(correspondingChunk), 
	m_costField(correspondingChunk->m_tileIDs.GetDimensions(), 0), 
	m_flowField(correspondingChunk->m_tileIDs.GetDimensions(), 0.f),
	m_gradient(correspondingChunk->m_tileIDs.GetDimensions(), Vec2::ZeroVector),
	m_consideredNodes(correspondingChunk->m_tileIDs.GetDimensions(), false)
{
}
