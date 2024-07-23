// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/Grid2D.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Renderer/VertexBuffer.h"



class Chunk;



//----------------------------------------------------------------------------------------------------------------------
class FlowFieldChunk
{
public:

	explicit FlowFieldChunk(Chunk* correspondingChunk);

	Chunk* m_chunk;

	Grid2D<uint8_t> m_costField;
	Grid2D<Vec2> m_gradient;
	Grid2D<float> m_flowField;
	Grid2D<bool> m_consideredNodes;
	VertexBuffer m_debugVBO;
};