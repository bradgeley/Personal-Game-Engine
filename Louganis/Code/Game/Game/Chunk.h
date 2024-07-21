// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/Grid2D.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Renderer/VertexBuffer.h"



struct WorldSettings;



//----------------------------------------------------------------------------------------------------------------------
class Chunk
{
public:

	void Generate(IntVec2 const& chunkCoords, WorldSettings const& worldSettings);
	void Destroy();

public:

	IntVec2 m_chunkCoords;
	AABB2 m_chunkBounds;
	Grid2D<uint8_t> m_tileIDs;
	VertexBuffer m_vbo;
#if defined(_DEBUG)
	VertexBuffer m_debugVBO;
#endif
};