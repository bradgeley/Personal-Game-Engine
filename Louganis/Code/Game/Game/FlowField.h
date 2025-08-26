// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include "FlowGenerationCoords.h"
#include <map>
#include <queue>



class FlowFieldChunk;



//----------------------------------------------------------------------------------------------------------------------
class FlowField
{
public:

	FlowFieldChunk* GetActiveChunk(IntVec2 const& chunkCoords);
	bool Seed(WorldCoords const& worldCoords);

	void HardReset();
	void SoftReset();
	void ResetConsideredCells();

public:

	std::priority_queue<FlowGenerationCoords> m_openList;
	std::map<IntVec2, FlowFieldChunk*> m_activeFlowFieldChunks;
};

