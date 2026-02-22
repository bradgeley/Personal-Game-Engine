// Bradley Christensen - 2022-2026
#include "FlowField.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
FlowField::FlowField() :
	m_costField(0),
	m_distanceField(StaticWorldSettings::s_maximumFlowDistance),
	m_gradient(Vec2::ZeroVector),
	m_consideredCells(false)
{
	if (g_renderer)
	{
		m_debugVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool FlowField::Seed(IntVec2 const& tileCoords)
{
	if (!m_distanceField.IsValidCoords(tileCoords))
	{
		return false;
	}

	return SeedUnsafe(tileCoords);
}



//----------------------------------------------------------------------------------------------------------------------
bool FlowField::SeedUnsafe(IntVec2 const& tileCoords)
{
	m_seeds.push_back(tileCoords);
	m_distanceField.Set(tileCoords, 0.f);
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
void FlowField::AddSeedsToOpenList()
{
	// Only iterate over the playable world. Any flow outside of that is irrelevant since entities cannot go there.
	for (IntVec2 const& seed : m_seeds)
	{
		m_openList.push(FlowGenerationCoords(seed, 0.f));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlowField::Reset()
{
	m_hasGeneratedFlow = false;
	m_seeds.clear();
	m_consideredCells.SetAll(false);
	m_costField.SetAll(0);
	m_distanceField.SetAll(StaticWorldSettings::s_maximumFlowDistance);
	m_gradient.SetAll(Vec2::ZeroVector);

	if (g_renderer)
	{
		g_renderer->ReleaseVertexBuffer(m_debugVBO);
		m_debugVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlowField::ResetConsideredCells()
{
	m_consideredCells.SetAll(false);
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 FlowField::GetFlowAtTileCoords(IntVec2 const& tileCoords) const
{
	if (m_hasGeneratedFlow && m_gradient.IsValidCoords(tileCoords))
	{
		return m_gradient.Get(tileCoords);
	}
	return Vec2::ZeroVector;
}



//----------------------------------------------------------------------------------------------------------------------
float FlowField::GetDistanceAtTileCoords(IntVec2 const& tileCoords) const
{
	if (m_hasGeneratedFlow && m_distanceField.IsValidCoords(tileCoords))
	{
		return m_distanceField.Get(tileCoords);
	}
	return 0.f;
}
