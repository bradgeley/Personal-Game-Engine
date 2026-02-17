// Bradley Christensen - 2022-2025
#include "SCWorld.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/Image.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "TileDef.h"
#include "WorldShaderCPU.h"
#include <queue>
#include <unordered_set>



//----------------------------------------------------------------------------------------------------------------------
IntVec2 s_neighborOffsets[8] =
{
	IntVec2(1, 0),
	IntVec2(-1, 0),
	IntVec2(0, 1),
	IntVec2(0, -1),
	IntVec2(1, 1),
	IntVec2(-1, -1),
	IntVec2(-1, 1),
	IntVec2(1, -1)
};



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::GenerateVBO()
{
	if (!m_isVBODirty)
	{
		return;
	}

	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	Vec2 lightmapUVsStepSize = Vec2(1.f / StaticWorldSettings::s_visibleWorldWidth, 1.f / StaticWorldSettings::s_visibleWorldHeight);
	Vec2 lightmapUVsHalfStepSize = lightmapUVsStepSize * 0.5f;

	// Get sprite sheet ready
	AssetID terrainID = g_assetManager->LoadSynchronous<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	GridSpriteSheet const* terrainSpriteSheet = g_assetManager->Get<GridSpriteSheet>(terrainID);
	ASSERT_OR_DIE(terrainSpriteSheet != nullptr, "SCWorld::GenerateMapVBO - Failed to load terrain sprite sheet");

	// Get VBO ready
	if (m_vbo == RendererUtils::InvalidID)
	{
		m_vbo = g_renderer->MakeVertexBuffer<TerrainVertex>(StaticWorldSettings::s_numVertsInVisibleWorld);
	}	
	
	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
	vbo.Resize(StaticWorldSettings::s_numVertsInVisibleWorld);

	Vec2 worldMins = Vec2(0.f, 0.f) + Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY);
	Vec2 worldDims = Vec2(StaticWorldSettings::s_worldWidth, StaticWorldSettings::s_worldWidth);
	Vec2 worldMaxs = worldMins + worldDims;
	AABB2 worldBounds = AABB2(worldMins, worldMaxs);

	ForEachVisibleTile([&](IntVec2 const& worldCoords, int visibleTileIndex)
	{
		int tileIndex = m_tiles.GetIndexForCoords(worldCoords);
		Tile& tile = m_tiles.GetRef(tileIndex);
		if (!tile.IsVertsDirty())
		{
			return true; // keep iterating
		}
		tile.SetVertsDirty(false);

		TileDef const* tileDef = TileDef::GetTileDef((uint8_t) tile.m_id);
		ASSERT_OR_DIE(tileDef != nullptr, "Chunk::Generate - Failed to find TileDef for tile id.");

		Vec2 mins = worldMins + Vec2(worldCoords.x, worldCoords.y) * StaticWorldSettings::s_tileWidth;
		Vec2 maxs = mins + tileDims;
		AABB2 uvs = terrainSpriteSheet->GetSpriteUVs(tileDef->m_spriteIndex);

		float staticLighting01 = static_cast<float>(tile.m_staticLighting) / 255.f;
		Rgba8 tint = tileDef->m_tint * staticLighting01;

		float z = 1.f;
		Vec3 bottomRightPoint = Vec3(maxs.x, mins.y, z);
		Vec3 topRightPoint = Vec3(maxs.x, maxs.y, z);
		Vec3 bottomLeftPoint = Vec3(mins.x, mins.y, z);
		Vec3 topLeftPoint = Vec3(mins.x, maxs.y, z);

		// UVs
		Vec2 const& topRightUVs = uvs.maxs;
		Vec2 const& bottomLeftUVs = uvs.mins;
		Vec2 bottomRightUVs = Vec2(uvs.maxs.x, uvs.mins.y);
		Vec2 topLeftUVs = Vec2(uvs.mins.x, uvs.maxs.y);

		// Lightmap UVs (Lightmap is only large enough to cover the visible world, so we need to use visible world relative coords)
		IntVec2 visibleWorldRelativeCoords = GetVisibleWorldRelativeCoords(worldCoords);
		Vec2 lightmapUVs = lightmapUVsHalfStepSize + Vec2(visibleWorldRelativeCoords.x, visibleWorldRelativeCoords.y) * lightmapUVsStepSize;

		// Write verts
		int firstVertIndex = visibleTileIndex * 6;
		TerrainVertex* firstVert = vbo.GetData<TerrainVertex>(firstVertIndex);
		*(firstVert) = TerrainVertex(bottomLeftPoint, tint, bottomLeftUVs, lightmapUVs);
		*(firstVert + 1) = TerrainVertex(bottomRightPoint, tint, bottomRightUVs, lightmapUVs);
		*(firstVert + 2) = TerrainVertex(topRightPoint, tint, topRightUVs, lightmapUVs);
		*(firstVert + 3) = TerrainVertex(bottomLeftPoint, tint, bottomLeftUVs, lightmapUVs);
		*(firstVert + 4) = TerrainVertex(topRightPoint, tint, topRightUVs, lightmapUVs);
		*(firstVert + 5) = TerrainVertex(topLeftPoint, tint, topLeftUVs, lightmapUVs);
		return true; // keep iterating
	});

	m_debugVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	VertexBuffer& debugVBO = *g_renderer->GetVertexBuffer(m_debugVBO);
	VertexUtils::AddVertsForWireGrid(debugVBO, worldBounds, IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), StaticWorldSettings::s_tileGridDebugDrawThickness, Rgba8::Black);
	VertexUtils::AddVertsForWireBox2D(debugVBO, worldBounds, 0.03f, Rgba8::Red);
	VertexUtils::AddVertsForArrow2D(debugVBO, Vec2(0.f, 0.f), Vec2(1.f, 0.f), StaticWorldSettings::s_worldOriginDebugDrawThickness, Rgba8::Red);
	VertexUtils::AddVertsForArrow2D(debugVBO, Vec2(0.f, 0.f), Vec2(0.f, 1.f), StaticWorldSettings::s_worldOriginDebugDrawThickness, Rgba8::Blue);

	m_isVBODirty = false;
	g_assetManager->Release(terrainID);
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::GenerateLightmap()
{
	if (m_lightmap == RendererUtils::InvalidID)
	{
		m_lightmap = g_renderer->MakeTexture();
	}

	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);

	Image image = Image(IntVec2(static_cast<int>(StaticWorldSettings::s_visibleWorldWidth), static_cast<int>(StaticWorldSettings::s_visibleWorldHeight)), Rgba8::TransparentBlack);
	image.SetName("WorldLightmap");
	Grid<Rgba8>& pixelGrid = image.GetPixelsRef();

	ForEachVisibleTile([&](IntVec2 const& worldCoords, int)
	{
		// Lightmap is only large enough to cover the visible world, so we need to use visible world relative coords
		IntVec2 visibleWorldRelativeCoords = GetVisibleWorldRelativeCoords(worldCoords);
		int tileIndex = m_tiles.GetIndexForCoords(worldCoords);
		Tile& tile = m_tiles.GetRef(tileIndex);
		pixelGrid.Set(visibleWorldRelativeCoords, Rgba8(tile.GetIndoorLighting255(), tile.GetOutdoorLighting255(), 0, 0));
		return true; // keep iterating
	});

	Texture* lightmapTex = g_renderer->GetTexture(m_lightmap);
	lightmapTex->CreateFromImage(image, false, false);
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::Shutdown()
{
	m_tiles.Clear();	
	g_renderer->ReleaseTexture(m_lightmap);
	g_renderer->ReleaseVertexBuffer(m_vbo);
	#if defined(_DEBUG)
	g_renderer->ReleaseVertexBuffer(m_debugVBO);
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsTileSolid(int tileIndex) const
{
	Tile const& tile = m_tiles.Get(tileIndex);
	return tile.IsSolid();
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsTileSolid(IntVec2 const& worldCoords) const
{
	Tile const& tile = m_tiles.Get(worldCoords);
	return tile.IsSolid();
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsTileVisible(int tileIndex) const
{
	IntVec2 tileCoords = m_tiles.GetCoordsForIndex(tileIndex);
	return IsTileVisible(tileCoords);
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsTileVisible(IntVec2 const& worldCoords) const
{
	return worldCoords.x >= StaticWorldSettings::s_visibleWorldBeginIndexX && worldCoords.x <= StaticWorldSettings::s_visibleWorldEndIndexX
		&& worldCoords.y >= StaticWorldSettings::s_visibleWorldBeginIndexY && worldCoords.y <= StaticWorldSettings::s_visibleWorldEndIndexY;
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsTileOnPath(int tileIndex) const
{
	Tile const& tile = m_tiles.Get(tileIndex);
	return tile.IsPath();
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsTileOnPath(IntVec2 const& worldCoords) const
{
	Tile const& tile = m_tiles.Get(worldCoords);
	return tile.IsPath();
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::DoesTileMatchTagQuery(IntVec2 const& worldCoords, TagQuery const& tagQuery) const
{
	Tile const& tile = m_tiles.Get(worldCoords);
	return tagQuery.Resolve(tile.m_tags);
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::CacheValidSpawnLocations()
{
	m_spawnLocations.clear();
	m_spawnLocations.reserve(16);

	ForEachEdgeTile([&](IntVec2 const& tileCoords)
	{
		Tile const& tile = m_tiles.Get(tileCoords);
		if (tile.IsPath())
		{
			m_spawnLocations.push_back(tileCoords);
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 SCWorld::GetRandomSpawnLocation() const
{
	if (m_spawnLocations.empty())
	{
		return Vec2();
	}

	int randomIndex = g_rng->GetRandomIntInRange(0, static_cast<int>(m_spawnLocations.size()) - 1);
	IntVec2 tileCoords = m_spawnLocations[randomIndex];
	AABB2 tileBounds = GetTileBounds(tileCoords);
	Vec2 randomLocationInBounds = g_rng->GetRandomVecInRange2D(tileBounds.mins, tileBounds.maxs);
	return randomLocationInBounds;
}



//----------------------------------------------------------------------------------------------------------------------
uint8_t SCWorld::GetTileCost(int tileIndex) const
{
	Tile tile = m_tiles.Get(tileIndex);
	TileDef const* tileDef = TileDef::GetTileDef(tile.m_id);
	if (tileDef)
	{
		return tileDef->m_cost;
	}
	return (uint8_t) 255;
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::SetTile(IntVec2 const& tileCoords, Tile const& tile)
{
	int index = m_tiles.GetIndexForCoords(tileCoords);
	return SetTile(index, tile);
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::SetTile(int tileIndex, Tile const& tile)
{
	if (!m_tiles.IsValidIndex(tileIndex))
	{
		return false;
	}

	Tile& existingTile = m_tiles.GetRef(tileIndex);
	if (existingTile == tile)
	{
		return true;
	}

	m_solidnessChanged = existingTile.IsSolid() != tile.IsSolid();
	m_isLightingDirty = true;
	m_isVBODirty = true;

	existingTile = tile;
	existingTile.SetLightingDirty(true);
	existingTile.SetVertsDirty(true);

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachVisibleTile(const std::function<bool(IntVec2 const&, int)>& func) const
{
	int visibleWorldRelativeTileIndex = 0;
	for (int y = StaticWorldSettings::s_visibleWorldBeginIndexY; y <= StaticWorldSettings::s_visibleWorldEndIndexY; ++y)
	{
		for (int x = StaticWorldSettings::s_visibleWorldBeginIndexX; x <= StaticWorldSettings::s_visibleWorldEndIndexX; ++x, ++visibleWorldRelativeTileIndex)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (!func(tileCoords, visibleWorldRelativeTileIndex))
			{
				return;
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachPlayableTile(const std::function<bool(IntVec2 const&)>& func) const
{
	for (int y = 0; y <= StaticWorldSettings::s_playableWorldEndIndexY; ++y)
	{
		for (int x = 0; x <= StaticWorldSettings::s_playableWorldEndIndexX; ++x)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (!func(tileCoords))
			{
				return;
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachPlayableTileOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(IntVec2 const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetCapsuleBounds(start, end, radius);

	ForEachPlayableTileOverlappingAABB(boundingBox, [&](IntVec2 const& worldCoords)
	{ 
		AABB2 tileBounds = GetTileBounds(worldCoords);
		if (GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, tileBounds))
		{
			if (!func(worldCoords))
			{
				return false;
			}
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachPlayableTileOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(IntVec2 const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetDiscBounds(pos, radius);

	ForEachPlayableTileOverlappingAABB(boundingBox, [&](IntVec2 const& worldCoords)
	{
		AABB2 tileBounds = GetTileBounds(worldCoords);
		if (GeometryUtils::DoesDiscOverlapAABB(pos, radius, tileBounds))
		{
			if (!func(worldCoords))
			{
				return false;
			}
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachPathTileOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(IntVec2 const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetDiscBounds(pos, radius);

	ForEachPlayableTileOverlappingAABB(boundingBox, [&](IntVec2 const& worldCoords)
	{
		if (!IsTileOnPath(worldCoords))
		{
			return true;
		}
		AABB2 tileBounds = GetTileBounds(worldCoords);
		if (GeometryUtils::DoesDiscOverlapAABB(pos, radius, tileBounds))
		{
			if (!func(worldCoords))
			{
				return false;
			}
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachPathTileInRange(Vec2 const& pos, float minRadius, float maxRadius, const std::function<bool(IntVec2 const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetDiscBounds(pos, maxRadius);

	ForEachPlayableTileOverlappingAABB(boundingBox, [&](IntVec2 const& worldCoords)
	{
		if (!IsTileOnPath(worldCoords))
		{
			return true;
		}
		AABB2 tileBounds = GetTileBounds(worldCoords);
		if (GeometryUtils::DoesRingOverlapAABB(pos, minRadius, maxRadius, tileBounds))
		{
			if (!func(worldCoords))
			{
				return false;
			}
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachPlayableTileOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 startGlobalTileCoords = GetTileCoordsAtWorldPosClamped(aabb.mins);
	IntVec2 endGlobalTileCoords = GetTileCoordsAtWorldPosClamped(aabb.maxs);

	int startX = MathUtils::Min(startGlobalTileCoords.x, endGlobalTileCoords.x);
	int endX = MathUtils::Max(startGlobalTileCoords.x, endGlobalTileCoords.x);

	int startY = MathUtils::Min(startGlobalTileCoords.y, endGlobalTileCoords.y);
	int endY = MathUtils::Max(startGlobalTileCoords.y, endGlobalTileCoords.y);

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (!func(tileCoords))
			{
				return;
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachEdgeTile(const std::function<bool(IntVec2 const&)>& func) const
{
	// Bottom row first, left to right
	for (int y = 0; y < StaticWorldSettings::s_numWorldEdgeTiles; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (!func(tileCoords))
			{
				return;
			}
		}
	}

	// Top row next, left to right
	for (int y = StaticWorldSettings::s_playableWorldEndIndexY - StaticWorldSettings::s_numWorldEdgeTiles + 1; y <= StaticWorldSettings::s_playableWorldEndIndexY; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (!func(tileCoords))
			{
				return;
			}
		}
	}

	// Left column, bottom to top
	for (int y = StaticWorldSettings::s_numWorldEdgeTiles; y <= StaticWorldSettings::s_playableWorldEndIndexY - StaticWorldSettings::s_numWorldEdgeTiles; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numWorldEdgeTiles; ++x)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (!func(tileCoords))
			{
				return;
			}
		}
	}

	// Right column, bottom to top
	for (int y = StaticWorldSettings::s_numWorldEdgeTiles; y <= StaticWorldSettings::s_playableWorldEndIndexY - StaticWorldSettings::s_numWorldEdgeTiles; ++y)
	{
		for (int x = StaticWorldSettings::s_playableWorldEndIndexX - StaticWorldSettings::s_numWorldEdgeTiles + 1; x <= StaticWorldSettings::s_playableWorldEndIndexX; ++x)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (!func(tileCoords))
			{
				return;
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsPointInsideSolidTile(Vec2 const& worldPos) const
{
	int tileIndex = GetTileIndexAtWorldPos(worldPos);
	if (!m_tiles.IsValidIndex(tileIndex))
	{
		return false;
	}
	return IsTileSolid(tileIndex);
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetVisibleWorldBounds() const
{
	AABB2 result;
	result.mins.x = StaticWorldSettings::s_visibleWorldMinsX;
	result.mins.y = StaticWorldSettings::s_visibleWorldMinsY;
	result.maxs.x = StaticWorldSettings::s_visibleWorldMaxsX;
	result.maxs.y = StaticWorldSettings::s_visibleWorldMaxsY;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetPlayableWorldBounds() const
{
	AABB2 result;
	result.mins.x = StaticWorldSettings::s_playableWorldMinsX;
	result.mins.y = StaticWorldSettings::s_playableWorldMinsY;
	result.maxs.x = StaticWorldSettings::s_playableWorldMaxsX;
	result.maxs.y = StaticWorldSettings::s_playableWorldMaxsY;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetVisibleWorldRelativeCoords(IntVec2 const& worldCoords) const
{
	return worldCoords - IntVec2(StaticWorldSettings::s_visibleWorldBeginIndexX, StaticWorldSettings::s_visibleWorldBeginIndexY);
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetTileCoordsAtWorldPos(Vec2 const& worldPos) const
{
	Vec2 relativeLocation = worldPos - Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY);
	Vec2 globalTileCoords = relativeLocation * StaticWorldSettings::s_oneOverTileWidth;
	return IntVec2(globalTileCoords.GetFloor());
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 SCWorld::GetTileCoordsAtWorldPosClamped(Vec2 const& worldPos) const
{
	Vec2 relativeLocation = worldPos - Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY);
	Vec2 globalTileCoords = relativeLocation * StaticWorldSettings::s_oneOverTileWidth;
	globalTileCoords.x = MathUtils::Clamp(globalTileCoords.x, 0.f, static_cast<float>(StaticWorldSettings::s_numTilesInRow - 1));
	globalTileCoords.y = MathUtils::Clamp(globalTileCoords.y, 0.f, static_cast<float>(StaticWorldSettings::s_numTilesInRow - 1));
	return IntVec2(globalTileCoords.GetFloor());
}



//----------------------------------------------------------------------------------------------------------------------
int SCWorld::GetTileIndexAtWorldPos(Vec2 const& worldPos) const
{
	IntVec2 tileCoords = GetTileCoordsAtWorldPos(worldPos);
	if (!m_tiles.IsValidCoords(tileCoords))
	{
		return -1;
	}
	int index = m_tiles.GetIndexForCoords(tileCoords);
	return index;
}



//----------------------------------------------------------------------------------------------------------------------
Tile const* SCWorld::GetTileAtWorldPos(Vec2 const& worldPos) const
{
	int tileIndex = GetTileIndexAtWorldPos(worldPos);
	if (m_tiles.IsValidIndex(tileIndex))
	{
		return &m_tiles.GetRef(tileIndex);
	}
	else
	{
		return nullptr;
	}
}



//----------------------------------------------------------------------------------------------------------------------
Tile* SCWorld::GetTileAtWorldPos(Vec2 const& worldPos)
{
	int tileIndex = GetTileIndexAtWorldPos(worldPos);
	if (m_tiles.IsValidIndex(tileIndex))
	{
		return &m_tiles.GetRef(tileIndex);
	}
	else
	{
		return nullptr;
	}
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBoundsAtWorldPos(Vec2 const& worldPos) const
{
	IntVec2 tileCoords = GetTileCoordsAtWorldPos(worldPos);
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	AABB2 tileBounds;
	tileBounds.mins = Vec2(tileCoords) * StaticWorldSettings::s_tileWidth + Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY);
	tileBounds.maxs = tileBounds.mins + tileDims;
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBounds(IntVec2 const& tileCoords) const
{
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	AABB2 tileBounds;
	tileBounds.mins = Vec2(tileCoords) * StaticWorldSettings::s_tileWidth + Vec2(StaticWorldSettings::s_worldOffsetX, StaticWorldSettings::s_worldOffsetY);
	tileBounds.maxs = tileBounds.mins + tileDims;
	return tileBounds;
}