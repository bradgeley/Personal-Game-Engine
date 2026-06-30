// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/TagQuery.h"
#include "Engine/DataStructures/BitArray.h"
#include "Engine/Math/FastGrid.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/RendererUtils.h"
#include "FlowGenerationCoords.h"
#include "WorldSettings.h"
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
class FlowField
{
public:

	FlowField(TagQuery tileTagQuery = TagQuery());

	bool Seed(IntVec2 const& tileCoords);
	bool SeedUnsafe(IntVec2 const& tileCoords); // Call if you've already validated the tile coords
	void AddSeedsToOpenList();

	void Reset();
	void ResetConsideredCells();

	Vec2 GetFlowAtTileCoords(IntVec2 const& tileCoords) const;
	float GetDistanceAtTileCoords(IntVec2 const& tileCoords) const;
	float GetInterpDistanceAtWorldPos(Vec2 const& worldPos) const;

public:

	std::priority_queue<FlowGenerationCoords> m_openList;

	TagQuery m_tileTagQuery; // Tiles that match this query will be considered for flow generation
	bool m_hasGeneratedFlow = false;
	std::vector<IntVec2> m_seeds;
	FastGrid<uint8_t, StaticWorldSettings::s_worldSizePowerOfTwo> m_costField;
	FastGrid<float, StaticWorldSettings::s_worldSizePowerOfTwo> m_distanceField;
	FastGrid<Vec2, StaticWorldSettings::s_worldSizePowerOfTwo> m_gradient;
	BitArray<StaticWorldSettings::s_numTilesInWorld> m_consideredCells;
};

