// Bradley Christensen - 2022-2025
#include "SCWorld.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Core/ErrorUtils.h"
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