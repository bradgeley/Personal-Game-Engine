// Bradley Christensen - 2022-2025
#include "SWorld.h"
#include "SCWorld.h"
#include "TileDef.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	TileDef::LoadFromXML();

	InitializeMap();
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Run(SystemContext const& context)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::EndFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown()
{

}




//----------------------------------------------------------------------------------------------------------------------
void SWorld::InitializeMap()
{
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();

	// Generate map tiles - for now just fill with grass, later will want to generate a more interesting map
	Tile backgroundTile = TileDef::GetDefaultTile("Grass");
	scWorld.m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), backgroundTile);

	CustomWorldSettings worldSettings;
	worldSettings.m_goalTileDistance = 10;

	Vec2 goalCenter = Vec2((float) StaticWorldSettings::s_visibleWorldMinsX + (float) worldSettings.m_goalTileDistance, (float) StaticWorldSettings::s_visibleWorldCenterY);
	AABB2 goalBounds = AABB2(goalCenter, (float) worldSettings.m_goalWidth * 0.5f, (float) worldSettings.m_goalWidth * 0.5f);
	goalBounds.Squeeze(0.01f); // Make sure we don't count tiles past the end of the AABB if it lines up perfectly with tile edges

	// Generate path tiles going out from the right side of the goal tiles
	AABB2 pathBounds = goalBounds;
	pathBounds.maxs.x = StaticWorldSettings::s_playableWorldMaxsX;
	scWorld.ForEachWorldCoordsOverlappingAABB(pathBounds, [&scWorld, &worldSettings](IntVec2 const& tileCoords)
	{
		Tile pathTile = TileDef::GetDefaultTile(worldSettings.m_pathTileName);
		pathTile.SetIsPath(true);
		scWorld.SetTile(tileCoords, pathTile);
		return true;
	});

	// Set goal tiles after path tiles, so they take precedence if they overlap
	scWorld.ForEachWorldCoordsOverlappingAABB(goalBounds, [&scWorld, &worldSettings](IntVec2 const& tileCoords)
	{
		Tile goalTile = TileDef::GetDefaultTile(worldSettings.m_goalTileName);
		goalTile.SetIsGoal(true);
		goalTile.SetIsPath(true);
		scWorld.SetTile(tileCoords, goalTile);
		return true;
	});
}
