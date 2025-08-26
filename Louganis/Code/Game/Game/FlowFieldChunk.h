// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/DataStructures/BitArray.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/FastGrid.h"
#include "Engine/Renderer/RendererUtils.h"
#include "WorldSettings.h"



class Chunk;
class SCWorld;
struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class FlowFieldChunk
{
public:

	explicit FlowFieldChunk(Chunk* chunk, SCWorld* world);
	~FlowFieldChunk();

	void HardReset();
	void SoftReset();

	IntVec2 GetChunkCoords() const;
	void SetDistanceFieldAt(IntVec2 const& localTileCoords, float distance);
	void GenerateCostField();
	void ResetConsideredCells();

	Chunk* GetChunk() const;
	AABB2 const& GetChunkBounds() const;
	bool IsTileSolid(IntVec2 const& localTileCoords) const;

protected:

	bool ChunkDestroyed(NamedProperties& args);

protected:

	SCWorld* m_world = nullptr;
	Chunk* m_chunk = nullptr;
	IntVec2 m_chunkCoords;
	AABB2 m_chunkBounds;

public:

	FastGrid<uint8_t, s_worldChunkSizePowerOfTwo> m_costField;
	FastGrid<float, s_worldChunkSizePowerOfTwo> m_distanceField;
	FastGrid<Vec2, s_worldChunkSizePowerOfTwo> m_gradient;
	BitArray<(1 << s_worldChunkSizePowerOfTwo) * (1 << s_worldChunkSizePowerOfTwo)> m_consideredCells;
	VertexBufferID m_debugVBO = RendererUtils::InvalidID;
};