// Bradley Christensen - 2024
#include "SCFlowField.h"
#include "FlowFieldChunk.h"



//----------------------------------------------------------------------------------------------------------------------
FlowFieldChunk* SCFlowField::GetActiveChunk(IntVec2 const& chunkCoords) const
{
	auto it = m_activeFlowFieldChunks.find(chunkCoords);
	if (it != m_activeFlowFieldChunks.end())
	{
		return it->second;
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void SCFlowField::Reset()
{
	for (auto& it : m_activeFlowFieldChunks)
	{
		auto chunk = it.second;
		chunk->m_consideredNodes.SetAll(false);
		chunk->m_costField.SetAll(0);
		chunk->m_distanceField.SetAll(0.f);
		chunk->m_gradient.SetAll(Vec2());
		chunk->m_debugVBO.ClearVerts();
	}
}
