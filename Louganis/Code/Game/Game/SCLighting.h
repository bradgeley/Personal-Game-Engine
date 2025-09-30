// Bradley Christensen - 2022-2025
#pragma once
#include "WorldCoords.h"
#include <unordered_set>



class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class SCLighting
{
public:	

	bool m_isLightingEnabled = true;
	std::unordered_set<WorldCoords> m_dirtyLightingTiles;
	std::unordered_set<Chunk*> m_dirtyChunks; // Lightmaps need to be rebuild
};

