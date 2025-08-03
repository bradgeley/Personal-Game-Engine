// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/Grid.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Renderer/VertexBuffer.h"



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

	Grid<uint8_t> m_costField;
	Grid<float> m_distanceField;
	Grid<Vec2> m_gradient;
	Grid<bool> m_consideredCells;
	VertexBuffer m_debugVBO;
};