// Bradley Christensen - 2022-2025
#include "SCWorld.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/Image.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/GeometryUtils.h"
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
void SCWorld::InitializeMap()
{
	// Generate map tiles - for now just fill with grass, later will want to generate a more interesting map
	Tile const& defaultTile = TileDef::GetDefaultTile("Grass");
	m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), defaultTile);

	GenerateLightmap();
	GenerateMapVBO();
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::GenerateMapVBO()
{
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	Vec2 lightmapUVsStepSize = Vec2(StaticWorldSettings::s_oneOverNumTilesInRow, StaticWorldSettings::s_oneOverNumTilesInRow);
	Vec2 lightmapUVsHalfStepSize = lightmapUVsStepSize * 0.5f;

	// Get sprite sheet ready
	AssetID terrainID = g_assetManager->LoadSynchronous<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	GridSpriteSheet const* terrainSpriteSheet = g_assetManager->Get<GridSpriteSheet>(terrainID);
	ASSERT_OR_DIE(terrainSpriteSheet != nullptr, "SCWorld::GenerateMapVBO - Failed to load terrain sprite sheet");

	// Get VBO ready
	if (m_vbo == RendererUtils::InvalidID)
	{
		m_vbo = g_renderer->MakeVertexBuffer<TerrainVertex>(StaticWorldSettings::s_numVertsInWorld);
	}	
	
	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(m_vbo);
	vbo.Resize(StaticWorldSettings::s_numVertsInWorld);

	Vec2 worldMins = Vec2(-StaticWorldSettings::s_worldHalfWidth, -StaticWorldSettings::s_worldHalfWidth);
	Vec2 worldDims = Vec2(StaticWorldSettings::s_worldWidth, StaticWorldSettings::s_worldWidth);
	Vec2 worldMaxs = worldMins + worldDims;
	AABB2 worldBounds = AABB2(worldMins, worldMaxs);

	int index = 0;
	for (int y = 0; y < StaticWorldSettings::s_numTilesInRow; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x, ++index)
		{
			Tile& tile = m_tiles.GetRef(index);
			TileDef const* tileDef = TileDef::GetTileDef((uint8_t) tile.m_id);
			ASSERT_OR_DIE(tileDef != nullptr, "Chunk::Generate - Failed to find TileDef for tile id.");

			Vec2 mins = worldMins + Vec2(x, y) * StaticWorldSettings::s_tileWidth;
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

			// Lightmap UVs
			Vec2 lightmapUVs = lightmapUVsHalfStepSize + Vec2(x, y) * lightmapUVsStepSize;

			// Write verts
			int firstVertIndex = index * 6;
			TerrainVertex* firstVert = vbo.GetData<TerrainVertex>(firstVertIndex);
			*(firstVert) = TerrainVertex(bottomLeftPoint, tint, bottomLeftUVs, lightmapUVs);
			*(firstVert + 1) = TerrainVertex(bottomRightPoint, tint, bottomRightUVs, lightmapUVs);
			*(firstVert + 2) = TerrainVertex(topRightPoint, tint, topRightUVs, lightmapUVs);
			*(firstVert + 3) = TerrainVertex(bottomLeftPoint, tint, bottomLeftUVs, lightmapUVs);
			*(firstVert + 4) = TerrainVertex(topRightPoint, tint, topRightUVs, lightmapUVs);
			*(firstVert + 5) = TerrainVertex(topLeftPoint, tint, topLeftUVs, lightmapUVs);
		}
	}

	#if defined(_DEBUG)
	m_debugVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	VertexBuffer& debugVBO = *g_renderer->GetVertexBuffer(m_debugVBO);
	VertexUtils::AddVertsForWireGrid(debugVBO, worldBounds, IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), 0.01f, Rgba8::Black);
	VertexUtils::AddVertsForWireBox2D(debugVBO, worldBounds, 0.03f, Rgba8::Red);
	#endif

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

	Image image = Image(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), Rgba8::TransparentBlack);
	Grid<Rgba8>& pixelGrid = image.GetPixelsRef();

	int index = 0;
	for (int y = 0; y < StaticWorldSettings::s_numTilesInRow; ++y)
	{
		for (int x = 0; x < StaticWorldSettings::s_numTilesInRow; ++x, ++index)
		{
			Tile& tile = m_tiles.GetRef(index);

			pixelGrid.Set(x, y, Rgba8(tile.GetIndoorLighting255(), tile.GetOutdoorLighting255(), 0, 0));
		}
	}

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
IntVec2 SCWorld::GetTileCoordsAtLocation(Vec2 const& worldLocation) const
{
	Vec2 globalTileCoords = worldLocation * StaticWorldSettings::s_oneOverTileWidth;
	return IntVec2(globalTileCoords.GetFloor());
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
void SCWorld::ForEachWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(IntVec2 const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetCapsuleBounds(start, end, radius);

	ForEachWorldCoordsOverlappingAABB(boundingBox, [&](IntVec2 const& worldCoords)
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
void SCWorld::ForEachWorldCoordsOverlappingCircle(Vec2 const& pos, float radius, const std::function<bool(IntVec2 const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetDiscBounds(pos, radius);

	ForEachWorldCoordsOverlappingAABB(boundingBox, [&](IntVec2 const& worldCoords)
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
void SCWorld::ForEachWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 startGlobalTileCoords = GetTileCoordsAtLocation(aabb.mins);
	IntVec2 endGlobalTileCoords = GetTileCoordsAtLocation(aabb.maxs);

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
void SCWorld::ForEachSolidWorldCoordsOverlappingAABB(AABB2 const& aabb, const std::function<bool(IntVec2 const&)>& func) const
{
	// Start in bot left, and iterate in a grid pattern
	IntVec2 startTileCoords = GetTileCoordsAtLocation(aabb.mins);
	IntVec2 endTileCoords = GetTileCoordsAtLocation(aabb.maxs);

	int startX = MathUtils::Min(startTileCoords.x, endTileCoords.x);
	int endX = MathUtils::Max(startTileCoords.x, endTileCoords.x);

	int startY = MathUtils::Min(startTileCoords.y, endTileCoords.y);
	int endY = MathUtils::Max(startTileCoords.y, endTileCoords.y);

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			IntVec2 tileCoords = IntVec2(x, y);
			if (IsTileSolid(tileCoords))
			{
				if (!func(tileCoords))
				{
					return;
				}
			}
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SCWorld::ForEachSolidWorldCoordsOverlappingCapsule(Vec2 const& start, Vec2 const& end, float radius, const std::function<bool(IntVec2 const&)>& func) const
{
	AABB2 boundingBox = GeometryUtils::GetCapsuleBounds(start, end, radius);

	ForEachSolidWorldCoordsOverlappingAABB(boundingBox, [&](IntVec2 const& coords)
	{
		AABB2 tileBounds = GetTileBounds(coords);
		if (!GeometryUtils::DoesCapsuleOverlapAABB(start, end, radius, tileBounds))
		{
			return true;
		}

		if (IsTileSolid(coords))
		{
			if (!func(coords))
			{
				return false;
			}
		}
		return true;
	});
}



//----------------------------------------------------------------------------------------------------------------------
bool SCWorld::IsPointInsideSolidTile(Vec2 const& worldPos) const
{
	IntVec2 tileCoords = GetTileCoordsAtLocation(worldPos);
	return IsTileSolid(tileCoords);
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBoundsAtWorldPos(Vec2 const& worldPos) const
{
	IntVec2 tileCoords = GetTileCoordsAtLocation(worldPos);
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	AABB2 tileBounds;
	tileBounds.mins = Vec2(tileCoords) * StaticWorldSettings::s_tileWidth;
	tileBounds.maxs = tileBounds.mins + tileDims;
	return tileBounds;
}



//----------------------------------------------------------------------------------------------------------------------
AABB2 SCWorld::GetTileBounds(IntVec2 const& tileCoords) const
{
	Vec2 tileDims = Vec2(StaticWorldSettings::s_tileWidth, StaticWorldSettings::s_tileWidth);
	AABB2 tileBounds;
	tileBounds.mins = Vec2(tileCoords) * StaticWorldSettings::s_tileWidth;
	tileBounds.maxs = tileBounds.mins + tileDims;
	return tileBounds;
}