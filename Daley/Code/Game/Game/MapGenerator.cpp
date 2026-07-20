// Bradley Christensen - 2022-2026
#include "MapGenerator.h"
#include "SCWorld.h"
#include "Tile.h"
#include "TileDef.h"
#include "WorldSettings.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Performance/ScopedTimer.h"



//----------------------------------------------------------------------------------------------------------------------
MapGenerator::MapGenerator(MapGeneratorDef const& mapGenDef, int seed) : m_def(&mapGenDef), m_seed(seed)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool MapGenerator::GenerateMap(SCWorld& world) const
{
	ScopedTimer timer("Generate Map");

	// Generate map tiles - for now just fill with grass, later will want to generate a more interesting map
	Tile backgroundTile = TileDef::GetDefaultTile("Grass");
	world.m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), backgroundTile);

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

	struct PerlinWorm
	{
		Vec2 m_pos;
		Vec2 m_dir;
		IntVec2 m_tileCoords;
	};

	std::vector<PerlinWorm> worms;
	worms.emplace_back(PerlinWorm{ goalCenter, Vec2(1.f, 0.f), world.GetTileCoordsAtWorldPos(goalCenter) });

	float numWormsProcessed = 0.f;
	int numSplits = 0;

	while (!worms.empty())
	{
		PerlinWorm worm = worms.front();
		worms.erase(worms.begin());

		numWormsProcessed += 1.f;

		int maxInterations = 10'000;
		int iterationCount = 0;
		float noiseLocation = static_cast<float>(world.m_worldSettings.m_seed + (999.f * numWormsProcessed));

		Tile pathTile = TileDef::GetDefaultTile(worldSettings.m_pathTileName);
		pathTile.SetIsPath(true);

		while (world.m_tiles.IsValidCoords(worm.m_tileCoords))
		{
			iterationCount++;
			if (iterationCount > maxInterations)
			{
				DevConsoleUtils::LogError("SWorld::InitializeMap - Reached max iterations while generating paths.");
				break;
			}

			noiseLocation += 0.01f;
			float noiseValue = GetPerlinNoise1D(noiseLocation, 0.5f, 5);
			Vec2 wormDir = worm.m_dir.GetRotated(noiseValue * 45.f);
			Vec2 nextWormLocation = worm.m_pos + wormDir;

			bool canSplit = numSplits < worldSettings.m_maxPathSplits;
			if (canSplit)
			{
				float splitNoise = GetNoiseZeroToOne1D(static_cast<int>(noiseLocation) + 1000, worldSettings.m_seed + iterationCount);
				bool didSplit = splitNoise < worldSettings.m_pathSplittiness;
				if (didSplit)
				{
					numSplits++;
					PerlinWorm splitWorm = worm;
					float splitRotation = iterationCount % 2 == 0 ? 45.f : -45.f;
					splitWorm.m_dir = worm.m_dir.GetRotated(splitRotation);
					worms.push_back(splitWorm);
				}
			}

			float capsuleRadius = 2.f;
			float capsuleRadiusNoise = 2.f * GetPerlinNoise2D_01(noiseLocation + 500.f, 25.f, 2);
			capsuleRadius *= capsuleRadiusNoise;
			capsuleRadius = MathUtils::Max(capsuleRadius, 1.f); // minimum radius of 1

			world.ForEachPlayableTileOverlappingCapsule(worm.m_pos, nextWormLocation, capsuleRadius, [&world, &pathTile](IntVec2 const& tileCoords)
														{
															Tile& tile = world.m_tiles.GetRef(tileCoords);
															if (tile.IsGoal())
															{
																return true;
															}
															tile = pathTile;
															return true;
														});

			worm.m_pos = nextWormLocation;
			worm.m_tileCoords = world.GetTileCoordsAtWorldPos(worm.m_pos);
		}
	}



	world.CacheValidSpawnLocations();
}
