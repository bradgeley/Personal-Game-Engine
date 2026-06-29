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

	IntVec2 m_dims = IntVec2::OneVector;
	TagQuery m_tileTagQuery;
};
