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
	// default for now

	// Set goal tiles
	Vec2 goalCenter = Vec2((float) StaticWorldSettings::s_visibleWorldMinsX + StaticWorldSettings::s_visibleWorldWidth * worldSettings.m_goalDistanceRatio, (float) StaticWorldSettings::s_visibleWorldCenterY);
	AABB2 goalBounds = AABB2(goalCenter, (float) worldSettings.m_goalWidth * 0.5f, (float) worldSettings.m_goalWidth * 0.5f);
	goalBounds.Squeeze(0.01f);

	scWorld.ForEachWorldCoordsOverlappingAABB(goalBounds, [&scWorld, &worldSettings](IntVec2 const& tileCoords)
	{
		Tile goalTile = TileDef::GetDefaultTile(worldSettings.m_goalTileName);
		goalTile.SetIsGoal(true);
		scWorld.SetTile(tileCoords, goalTile);
		return true;
	});
}
