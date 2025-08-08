// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/FastGrid.h"
#include "Engine/Math/AABB2.h"
#include "Engine/DataStructures/BitArray.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "WorldSettings.h"



class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class FlowFieldChunk
{
public:

	explicit FlowFieldChunk(Chunk* chunk);

	void HardReset();
	void SoftReset();

	IntVec2 GetChunkCoords() const;
	void SetDistanceFieldAt(IntVec2 const& localTileCoords, float distance);
	void GenerateCostField();
	void ResetConsideredCells();

public:

	Chunk* m_chunk;

	FastGrid<uint8_t, s_worldChunkSizePowerOfTwo> m_costField;
	FastGrid<float, s_worldChunkSizePowerOfTwo> m_distanceField;
	FastGrid<Vec2, s_worldChunkSizePowerOfTwo> m_gradient;
	BitArray<(1 << s_worldChunkSizePowerOfTwo) * (1 << s_worldChunkSizePowerOfTwo)> m_consideredCells;
	VertexBuffer m_debugVBO;
};