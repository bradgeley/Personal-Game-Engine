// Bradley Christensen - 2022-2025
#include "SWorld.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "TileDef.h"
#include "WorldRaycast.h"
#include "Engine/DataStructures/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	TileDef::LoadFromXML();

	DevConsoleUtils::AddDevConsoleCommand("GenerateMap", &SWorld::GenerateMap, "seed", DevConsoleArgType::Int);

	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	GenerateMap(scWorld);
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Run(SystemContext const&)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::EndFrame()
{

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown()
{
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.Shutdown();

	DevConsoleUtils::RemoveDevConsoleCommand("GenerateMap", &SWorld::GenerateMap);
}



//----------------------------------------------------------------------------------------------------------------------
bool SWorld::GenerateMap(NamedProperties& params)
{
	int seed = params.Get<int>("seed", 0);

	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	world.m_worldSettings.m_seed = seed;

	GenerateMap(world);

	SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
	scFlowField.m_toGoalFlowField.Reset();

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::GenerateMap(SCWorld& world)
{
	ScopedTimer timer("Generate Map");

	// Generate map tiles - for now just fill with grass, later will want to generate a more interesting map
	Tile backgroundTile = TileDef::GetDefaultTile("Grass");
	world.m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), backgroundTile);

	CustomWorldSettings worldSettings;
	worldSettings.m_goalTileDistance = 10;

	Vec2 goalCenter = Vec2((float) StaticWorldSettings::s_visibleWorldMinsX + (float) worldSettings.m_goalTileDistance, (float) StaticWorldSettings::s_visibleWorldCenterY);
	AABB2 goalBounds = AABB2(goalCenter, (float) worldSettings.m_goalWidth * 0.5f, (float) worldSettings.m_goalWidth * 0.5f);
	goalBounds.Squeeze(0.01f); // Make sure we don't count tiles past the end of the AABB if it lines up perfectly with tile edges

	// Set goal tiles after path tiles, so they take precedence if they overlap
	Tile goalTile = TileDef::GetDefaultTile(worldSettings.m_goalTileName);
	goalTile.SetIsGoal(true);
	goalTile.SetIsPath(true);

	world.ForEachPlayableTileOverlappingAABB(goalBounds, [&world, &worldSettings, &goalTile](IntVec2 const& tileCoords)
	{
		world.SetTile(tileCoords, goalTile);
		return true;
	});

	// Perlin worms to make paths
	Vec2 wormLocation = goalCenter;
	Vec2 wormGeneralDirection = Vec2(1.f, 0.f);
	IntVec2 wormTileCoords = world.GetTileCoordsAtWorldPos(wormLocation);
	Tile& tile = world.m_tiles.GetRef(wormTileCoords);

	int maxInterations = 10'000;
	int iterationCount = 0;
	float noiseLocation = static_cast<float>(world.m_worldSettings.m_seed + 1.f);

	Tile pathTile = TileDef::GetDefaultTile(worldSettings.m_pathTileName);
	pathTile.SetIsPath(true);

	while (world.m_tiles.IsValidCoords(wormTileCoords))
	{
		iterationCount++;
		if (iterationCount > maxInterations)
		{
			DevConsoleUtils::LogError("SWorld::InitializeMap - Reached max iterations while generating paths.");
			break;
		}

		noiseLocation += 0.01f;
		float noiseValue = GetPerlinNoise1D(noiseLocation, 0.5f, 5);
		Vec2 wormDir = wormGeneralDirection.GetRotated(noiseValue * 90.f);
		Vec2 nextWormLocation = wormLocation + wormDir * 1.f;

		world.ForEachPlayableTileOverlappingCapsule(wormLocation, nextWormLocation, 2.f, [&world, &pathTile](IntVec2 const& tileCoords)
		{
			Tile& tile = world.m_tiles.GetRef(tileCoords);
			if (tile.IsGoal())
			{
				return true;
			}
			tile = pathTile;
			return true;
		});

		wormLocation = nextWormLocation;
		wormTileCoords = world.GetTileCoordsAtWorldPos(wormLocation);
	}

	world.CacheValidSpawnLocations();
}
