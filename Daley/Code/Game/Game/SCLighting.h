// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include <unordered_set>



class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class SCLighting
{
public:	

	bool m_isLightingEnabled = true;
	std::unordered_set<IntVec2> m_dirtyLightingTiles;
};

