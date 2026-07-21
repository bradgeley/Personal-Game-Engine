// Bradley Christensen - 2022-2026
#include "MapGeneratorComponent.h"
#include "BiomeGeneratorDef.h"
#include "MapGenerator.h"
#include "MapGeneratorComponentDef.h"
#include "SCWorld.h"
#include "TileDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent::MapGeneratorComponent(MapGeneratorComponentDef const& def) : m_def(&def)
{

}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent::TileSelectorComponent(MapGeneratorComponentDef const& def) : MapGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool TileSelectorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
NoiseSelectorComponent::NoiseSelectorComponent(MapGeneratorComponentDef const& def) : TileSelectorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool NoiseSelectorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	if (!TileSelectorComponent::Generate(generator, world))
	{
		return false;
	}
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
NoiseRangeSelectorComponent::NoiseRangeSelectorComponent(MapGeneratorComponentDef const& def) : NoiseSelectorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool NoiseRangeSelectorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	if (!NoiseSelectorComponent::Generate(generator, world))
	{
		return false;
	}
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool NoiseRangeSelectorComponent::SelectTiles(MapGenerator& generator, SCWorld& world)
{
	NoiseRangeSelectorComponentDef const* def = dynamic_cast<NoiseRangeSelectorComponentDef const*>(m_def);
	if (!def)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a NoiseRangeSelectorComponentDef");
	}

	NoiseParams const& params = def->m_noiseParams;
	Vec2 const& noiseRange = def->m_noiseRange;
	int seed = generator.GetSeed() + params.m_seedOffset;

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		float noiseValue = Noise::GetPerlinNoise2D((float) tileCoords.x, (float) tileCoords.y, params.m_scale, params.m_numOctaves, params.m_octavePersistence, params.m_octaveScale, params.m_renormalize, seed);
		float rangeMappedNoise = MathUtils::RangeMapClamped(noiseValue, -1.f, 1.f, params.m_outputRange.x, params.m_outputRange.y);

		if (rangeMappedNoise >= noiseRange.x && rangeMappedNoise <= noiseRange.y)
		{
			m_selectedTiles.push_back(tileCoords);
		}
		return true; // Continue iterating
	});

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
NoisePeakSelectorComponent::NoisePeakSelectorComponent(MapGeneratorComponentDef const& def) : NoiseSelectorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool NoisePeakSelectorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	if (!NoiseSelectorComponent::Generate(generator, world))
	{
		return false;
	}
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
bool NoisePeakSelectorComponent::SelectTiles(MapGenerator& generator, SCWorld& world)
{
	NoisePeakSelectorComponentDef const* def = dynamic_cast<NoisePeakSelectorComponentDef const*>(m_def);
	if (!def)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a NoisePeakSelectorComponentDef");
	}

	NoiseParams const& params = def->m_noiseParams;
	int seed = generator.GetSeed() + params.m_seedOffset;

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		float noiseValue = Noise::GetPerlinNoise2D((float) tileCoords.x, (float) tileCoords.y, params.m_scale, params.m_numOctaves, params.m_octavePersistence, params.m_octaveScale, params.m_renormalize, seed);
		float rangeMappedNoise = MathUtils::RangeMapClamped(noiseValue, -1.f, 1.f, params.m_outputRange.x, params.m_outputRange.y);

		bool isNoiseValuePeak = true;
		world.ForEachPlayableNeighboringTile(tileCoords, [&](IntVec2 const& neighborCoords)
		{
			float neighborNoiseValue = Noise::GetPerlinNoise2D((float) neighborCoords.x, (float) neighborCoords.y, params.m_scale, params.m_numOctaves, params.m_octavePersistence, params.m_octaveScale, params.m_renormalize, seed);
			float rangeMappedNeighborNoise = MathUtils::RangeMapClamped(neighborNoiseValue, -1.f, 1.f, params.m_outputRange.x, params.m_outputRange.y);
			if (rangeMappedNoise <= rangeMappedNeighborNoise)
			{
				// One neighbor is larger, so this is not a peak
				isNoiseValuePeak = false;
				return false;
			}
			return true; // Continue checking neighbors
		});

		if (isNoiseValuePeak)
		{
			m_selectedTiles.push_back(tileCoords);
		}
		return true; // Continue iterating
	});

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
BiomeGeneratorComponent::BiomeGeneratorComponent(MapGeneratorComponentDef const& def) : MapGeneratorComponent(def)
{
	BiomeGeneratorComponentDef const* biomeDef = dynamic_cast<BiomeGeneratorComponentDef const*>(m_def);
	if (!biomeDef)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a BiomeGeneratorComponentDef");
	}

	BiomeGeneratorDef const* biomeGenDef = BiomeGeneratorDef::GetBiomeGeneratorDef(biomeDef->m_biomeName);
	if (!biomeGenDef)
	{
		ERROR_AND_DIE(StringUtils::StringF("BiomeGeneratorDef not found for biome: %s", biomeDef->m_biomeName.ToCStr()));
	}

	for (MapGeneratorComponentDef const* componentDef : biomeGenDef->m_generatorComponentDefs)
	{
		MapGeneratorComponent* component = componentDef->MakeComponentInstance();
		if (!component)
		{
			ERROR_AND_DIE(StringUtils::StringF("Failed to create MapGeneratorComponent for biome: %s", biomeDef->m_biomeName.ToCStr()));
		}
		m_biomeComponents.push_back(component);
	}
}



//----------------------------------------------------------------------------------------------------------------------
BiomeGeneratorComponent::~BiomeGeneratorComponent()
{
	for (MapGeneratorComponent* component : m_biomeComponents)
	{
		delete component;
	}
	m_biomeComponents.clear();
}



//----------------------------------------------------------------------------------------------------------------------
bool BiomeGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	BiomeGeneratorComponentDef const* biomeDef = dynamic_cast<BiomeGeneratorComponentDef const*>(m_def);
	if (!biomeDef)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a BiomeGeneratorComponentDef");
	}

	BiomeGeneratorDef const* biomeGenDef = BiomeGeneratorDef::GetBiomeGeneratorDef(biomeDef->m_biomeName);
	if (!biomeGenDef)
	{
		ERROR_AND_DIE(StringUtils::StringF("BiomeGeneratorDef not found for biome: %s", biomeDef->m_biomeName.ToCStr()));
	}

	Tile backgroundTile = TileDef::GetDefaultTile(biomeGenDef->m_baseTile);
	world.m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), backgroundTile);

	for (MapGeneratorComponent* component : m_biomeComponents)
	{
		if (!component->Generate(generator, world))
		{
			return false; // If any component fails, the generation fails
		}
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
DiscreteGeneratorComponent::DiscreteGeneratorComponent(MapGeneratorComponentDef const& def) : MapGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool DiscreteGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
TileGeneratorComponent::TileGeneratorComponent(MapGeneratorComponentDef const& def) : DiscreteGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool TileGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	if (!DiscreteGeneratorComponent::Generate(generator, world))
	{
		return false;
	}

	TileGeneratorComponentDef const* def = dynamic_cast<TileGeneratorComponentDef const*>(m_def);
	if (!def)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a TileGeneratorComponentDef");
	}

	TileDef const* tileDef = TileDef::GetTileDef(def->m_tileName);
	if (!tileDef)
	{
		ERROR_AND_DIE(StringUtils::StringF("TileDef not found for tile: %s", def->m_tileName.ToCStr()));
	}

	Tile defaultTile = tileDef->GetDefaultTile(def->m_tileName);

	TileSelectorComponent* selector = generator.GetTileSelectorComponentByName(def->m_tileSelectorName);
	if (!selector)
	{
		ERROR_AND_DIE(StringUtils::StringF("TileSelectorComponent not found: %s", def->m_tileSelectorName.ToCStr()));
	}

	selector->SelectTiles(generator, world);

	for (IntVec2 const& tileCoords : selector->m_selectedTiles)
	{
		world.SetTile(tileCoords, defaultTile);
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
EntityGeneratorComponent::EntityGeneratorComponent(MapGeneratorComponentDef const& def) : DiscreteGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool EntityGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	// TODO:
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
GoalGeneratorComponent::GoalGeneratorComponent(MapGeneratorComponentDef const& def) : MapGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool GoalGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
DiscGoalGeneratorComponent::DiscGoalGeneratorComponent(MapGeneratorComponentDef const& def) : GoalGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool DiscGoalGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	if (!GoalGeneratorComponent::Generate(generator, world))
	{
		return false;
	}

	DiscGoalGeneratorComponentDef const* discDef = dynamic_cast<DiscGoalGeneratorComponentDef const*>(m_def);
	if (!discDef)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a DiscGoalGeneratorComponentDef");
	}

	Vec2 alignment = discDef->m_mapAlignment;
	float goalCenterX = StaticWorldSettings::s_visibleWorldMinsX + (StaticWorldSettings::s_visibleWorldWidth * alignment.x);
	float goalCenterY = StaticWorldSettings::s_visibleWorldMinsY + (StaticWorldSettings::s_visibleWorldHeight * alignment.y);

	float goalRadius = discDef->m_radius;
	goalRadius *= 0.99f;

	Vec2 goalCenter = Vec2(goalCenterX, goalCenterY);

	Name tileName = discDef->m_tileName;

	// Set goal tiles after path tiles, so they take precedence if they overlap
	Tile goalTile = TileDef::GetDefaultTile(tileName);
	goalTile.SetIsGoal(true);
	goalTile.SetIsPath(true);

	world.ForEachPlayableTileOverlappingCircle(goalCenter, goalRadius, [&world, &goalTile](IntVec2 const& tileCoords)
	{
		world.SetTile(tileCoords, goalTile);
		return true;
	});

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
RectGoalGeneratorComponent::RectGoalGeneratorComponent(MapGeneratorComponentDef const& def) : GoalGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool RectGoalGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	if (!GoalGeneratorComponent::Generate(generator, world))
	{
		return false;
	}

	RectGoalGeneratorComponentDef const* rectDef = dynamic_cast<RectGoalGeneratorComponentDef const*>(m_def);
	if (!rectDef)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a RectGoalGeneratorComponentDef");
	}

	Vec2 alignment = rectDef->m_mapAlignment;
	float goalCenterX = StaticWorldSettings::s_visibleWorldMinsX + (StaticWorldSettings::s_visibleWorldWidth * alignment.x);
	float goalCenterY = StaticWorldSettings::s_visibleWorldMinsY + (StaticWorldSettings::s_visibleWorldHeight * alignment.y);

	IntVec2 goalDims = rectDef->m_dims;

	Vec2 goalCenter = Vec2(goalCenterX, goalCenterY);
	AABB2 goalBounds = AABB2(goalCenter, (float) goalDims.x * 0.5f, (float) goalDims.y * 0.5f);
	goalBounds.Squeeze(0.01f); // Make sure we don't count tiles past the end of the AABB if it lines up perfectly with tile edges

	Name tileName = rectDef->m_tileName;

	// Set goal tiles after path tiles, so they take precedence if they overlap
	Tile goalTile = TileDef::GetDefaultTile(tileName);
	goalTile.SetIsGoal(true);
	goalTile.SetIsPath(true);

	world.ForEachPlayableTileOverlappingAABB(goalBounds, [&world, &goalTile](IntVec2 const& tileCoords)
	{
		world.SetTile(tileCoords, goalTile);
		return true;
	});

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
PerlinWormPathGeneratorComponent::PerlinWormPathGeneratorComponent(MapGeneratorComponentDef const& def) : MapGeneratorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool PerlinWormPathGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	struct PerlinWorm
	{
		Vec2 m_pos;
		Vec2 m_dir;
		IntVec2 m_tileCoords;
	};

	PerlinWormPathGeneratorComponentDef const* perlinWormDef = dynamic_cast<PerlinWormPathGeneratorComponentDef const*>(m_def);
	if (!perlinWormDef)
	{
		ERROR_AND_DIE("MapGeneratorComponentDef is not a PerlinWormPathGeneratorComponentDef");
	}

	int seed = generator.GetSeed();

	Vec2 goalLocation = Vec2::ZeroVector;	
	world.ForEachVisibleTile([&world, &goalLocation](IntVec2 const& tileCoords, int)
	{
		Tile const& tile = world.m_tiles.GetRef(tileCoords);
		if (tile.IsGoal())
		{
			goalLocation = world.GetTileBounds(tileCoords).GetCenter();
			return false; // Stop iterating once we find a goal tile
		}
		return true;
	});

	Tile pathTile = TileDef::GetDefaultTile(perlinWormDef->m_tileName);
	pathTile.SetIsPath(true);

	std::vector<PerlinWorm> worms;
	worms.emplace_back(PerlinWorm{ goalLocation, perlinWormDef->m_startDir, world.GetTileCoordsAtWorldPos(goalLocation) });

	float numWormsProcessed = 0.f;
	int numSplits = 0;

	while (!worms.empty())
	{
		PerlinWorm worm = worms.front();
		worms.erase(worms.begin());

		numWormsProcessed += 1.f;

		int maxIterations = 100'000;
		int iterationCount = 0;
		float noiseLocation = static_cast<float>(seed + (999.f * numWormsProcessed));

		while (world.m_tiles.IsValidCoords(worm.m_tileCoords))
		{
			iterationCount++;
			if (iterationCount > maxIterations)
			{
				DevConsoleUtils::LogError("SWorld::InitializeMap - Reached max iterations while generating paths.");
				break;
			}

			noiseLocation += 0.01f;
			float noiseValue = Noise::GetPerlinNoise1D(noiseLocation, 0.5f, 5);
			Vec2 wormDir = worm.m_dir.GetRotated(noiseValue * 45.f); // todo: parameterize
			Vec2 nextWormLocation = worm.m_pos + wormDir;

			bool canSplit = numSplits < perlinWormDef->m_maxSplits;
			if (canSplit)
			{
				float splitNoise = Noise::GetNoiseZeroToOne1D(static_cast<int>(noiseLocation) + 1000, seed + iterationCount);
				bool didSplit = splitNoise < perlinWormDef->m_splitChance;
				if (didSplit)
				{
					numSplits++;
					PerlinWorm splitWorm = worm;
					float splitRotation = iterationCount % 2 == 0 ? perlinWormDef->m_splitAngleDeg : -perlinWormDef->m_splitAngleDeg;
					splitWorm.m_dir = worm.m_dir.GetRotated(splitRotation);
					worms.push_back(splitWorm);
				}
			}

			float capsuleRadius = perlinWormDef->m_thicknessRange.x;
			float capsuleVariance = perlinWormDef->m_thicknessRange.y - perlinWormDef->m_thicknessRange.x;
			float capsuleRadiusNoise = capsuleVariance * Noise::GetPerlinNoise2D_01(noiseLocation + 500.f, 25.f, 2);
			capsuleRadius += capsuleRadiusNoise;
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

	return true;
}
