// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/DataStructures/BitArray.h"
#include "Engine/Math/FastGrid.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/RendererUtils.h"
#include "FlowGenerationCoords.h"
#include "WorldSettings.h"
#include <map>
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
class FlowField
{
public:

	bool Seed(IntVec2 const& tileCoords);

	void HardReset();
	void SoftReset();
	void ResetConsideredCells();

	Vec2 GetFlowAtTileCoords(IntVec2 const& tileCoords) const;

public:

	std::priority_queue<FlowGenerationCoords> m_openList;
	FastGrid<uint8_t, StaticWorldSettings::s_worldSizePowerOfTwo> m_costField;
	FastGrid<float, StaticWorldSettings::s_worldSizePowerOfTwo> m_distanceField;
	FastGrid<Vec2, StaticWorldSettings::s_worldSizePowerOfTwo> m_gradient;
	BitArray<StaticWorldSettings::s_numTilesInWorld> m_consideredCells;
	VertexBufferID m_debugVBO = RendererUtils::InvalidID;
};

