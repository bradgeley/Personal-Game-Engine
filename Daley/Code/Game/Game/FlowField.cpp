// Bradley Christensen - 2022-2026
#include "FlowField.h"
#include "SCWorld.h"
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



//----------------------------------------------------------------------------------------------------------------------
float FlowField::GetInterpDistanceAtWorldPos(Vec2 const& worldPos) const
{
	if (m_hasGeneratedFlow)
	{
		Vec2 relativeLocation = worldPos - Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY);
		Vec2 globalGridLocation = relativeLocation * StaticWorldSettings::s_oneOverTileWidth;
		IntVec2 centerTileCoords = IntVec2(globalGridLocation.GetFloor());
		if (m_distanceField.IsValidCoords(centerTileCoords))
		{
			Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
			AABB2 tileBounds;
			tileBounds.mins = Vec2(centerTileCoords) * StaticWorldSettings::s_tileWidth;
			tileBounds.maxs = tileBounds.mins + tileDims;

			Vec2 pivotPt = tileBounds.GetCenter();
			int quadrantX = globalGridLocation.x > (float) pivotPt.x ? 0 : -1;
			int quadrantY = globalGridLocation.y > (float) pivotPt.y ? 0 : -1;

			IntVec2 botLeftTileCoords = centerTileCoords + IntVec2(quadrantX, quadrantY);
			IntVec2 botRightTileCoords = botLeftTileCoords + IntVec2(1, 0);
			IntVec2 topLeftTileCoords = botLeftTileCoords + IntVec2(0, 1);
			IntVec2 topRightTileCoords = botLeftTileCoords + IntVec2(1, 1);

			Vec2 botLeftTileCenter = Vec2(botLeftTileCoords) + Vec2(0.5f, 0.5f);
			float tx = MathUtils::Clamp01F(globalGridLocation.x - botLeftTileCenter.x);
			float ty = MathUtils::Clamp01F(globalGridLocation.y - botLeftTileCenter.y);

			float botLeftDistance = m_distanceField.Get(botLeftTileCoords);
			float botRightDistance = m_distanceField.Get(botRightTileCoords);
			float topLeftDistance = m_distanceField.Get(topLeftTileCoords);
			float topRightDistance = m_distanceField.Get(topRightTileCoords);

			bool isBotLeftValid = botLeftDistance != StaticWorldSettings::s_maximumFlowDistance && m_distanceField.IsValidCoords(botLeftTileCoords);
			bool isBotRightValid = botRightDistance != StaticWorldSettings::s_maximumFlowDistance && m_distanceField.IsValidCoords(botRightTileCoords);
			bool isTopLeftValid = topLeftDistance != StaticWorldSettings::s_maximumFlowDistance && m_distanceField.IsValidCoords(topLeftTileCoords);
			bool isTopRightValid = topRightDistance != StaticWorldSettings::s_maximumFlowDistance && m_distanceField.IsValidCoords(topRightTileCoords);

			float totalWeight = 0.f;
			float distanceSum = 0.f;
			if (isBotLeftValid)
			{
				float botLeftWeight = (1.f - tx) * (1.f - ty);
				distanceSum += botLeftDistance * botLeftWeight;
				totalWeight += botLeftWeight;
			}
			if (isBotRightValid)
			{
				float botRightWeight = tx * (1.f - ty);
				distanceSum += botRightDistance * botRightWeight;
				totalWeight += botRightWeight;
			}
			if (isTopLeftValid)
			{
				float topLeftWeight = (1.f - tx) * ty;
				distanceSum += topLeftDistance * topLeftWeight;
				totalWeight += topLeftWeight;
			}
			if (isTopRightValid)
			{
				float topRightWeight = tx * ty;
				distanceSum += topRightDistance * topRightWeight;
				totalWeight += topRightWeight;
			}

			if (totalWeight > 0.f)
			{
				return distanceSum / totalWeight;
			}

			return m_distanceField.Get(centerTileCoords);
		}
	}
	return 0.0f;
}
