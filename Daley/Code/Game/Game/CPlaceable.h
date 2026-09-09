// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Core/TagQuery.h"



//----------------------------------------------------------------------------------------------------------------------
struct CPlaceable
{
public:

    CPlaceable() = default;
    CPlaceable(void const* xmlElement);

public:

	float m_costOfPurchase = 10.f;
	IntVec2 m_dims = IntVec2::OneVector;
	IntVec2 m_botLeftTile = IntVec2::ZeroVector;
	TagQuery m_tileTagQuery;
};
