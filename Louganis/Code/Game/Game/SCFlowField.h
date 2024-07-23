// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/IntVec2.h"
#include <map>



class FlowFieldChunk;



//----------------------------------------------------------------------------------------------------------------------
class SCFlowField
{
public:

	FlowFieldChunk* GetActiveChunk(IntVec2 const& chunkCoords) const;
	void Reset();

public:

	IntVec2 m_lastKnownPlayerLocation = IntVec2(INT_MAX, INT_MAX);
	std::map<IntVec2, FlowFieldChunk*> m_activeFlowFieldChunks;
};

