// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Core/TagQuery.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



//----------------------------------------------------------------------------------------------------------------------
struct TowerPlacementInfo
{
	Name m_towerName = "Rainbow";
	Vec2 m_worldPos = Vec2::ZeroVector;
	IntVec2 m_botLeftTileCoords = IntVec2::ZeroVector;
	IntVec2 m_topRightTileCoords = IntVec2::ZeroVector;
	IntVec2 m_dims = IntVec2::OneVector;
	TagQuery m_tileTagQuery;
};