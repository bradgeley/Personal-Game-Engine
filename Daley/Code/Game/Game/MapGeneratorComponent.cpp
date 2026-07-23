// Bradley Christensen - 2022-2026
#include "MapGeneratorComponent.h"
#include "BiomeDef.h"
#include "EntityDef.h"
#include "MapGenerator.h"
#include "MapGeneratorComponentDef.h"
#include "SCWorld.h"
#include "SInput.h" // todo: Weird include for function, move it later
#include "TileDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Performance/ScopedTimer.h"



//----------------------------------------------------------------------------------------------------------------------
NoiseRangeSelectorComponent::NoiseRangeSelectorComponent(NoiseRangeSelectorComponentDef const& def) : m_noiseRange(def.m_noiseRange), m_params(def.m_noiseParams)
{
	m_name = def.m_name;
	m_tagQuery = def.m_tagQuery;
}



//----------------------------------------------------------------------------------------------------------------------
void NoiseRangeSelectorComponent::ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func)
{
	ScopedTimer timer(StringUtils::StringF("NoiseRangeSelectorComponent::ForEachSelectedTile - %s", m_name.ToCStr()));

	int seed = generator.GetSeed() + m_params.m_seedOffset;

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		if (!world.DoesTileMatchTagQuery(tileCoords, m_tagQuery))
		{
			return true;
		}

		float noiseValue = Noise::GetPerlinNoise2D((float) tileCoords.x, (float) tileCoords.y, m_params.m_scale, m_params.m_numOctaves, m_params.m_octavePersistence, m_params.m_octaveScale, m_params.m_renormalize, seed);
		float rangeMappedNoise = MathUtils::RangeMapClamped(noiseValue, -1.f, 1.f, m_params.m_outputRange.x, m_params.m_outputRange.y);

		if (rangeMappedNoise >= m_noiseRange.x && rangeMappedNoise <= m_noiseRange.y)
		{
			if (!func(tileCoords))
			{
				return false;
			}
		}
		return true; // Continue iterating
	});
}



//----------------------------------------------------------------------------------------------------------------------
NoisePeakSelectorComponent::NoisePeakSelectorComponent(NoisePeakSelectorComponentDef const& def) : m_params(def.m_noiseParams)
{
	m_name = def.m_name;
	m_tagQuery = def.m_tagQuery;
}



//----------------------------------------------------------------------------------------------------------------------
void NoisePeakSelectorComponent::ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func)
{
	ScopedTimer timer(StringUtils::StringF("NoisePeakSelectorComponent::ForEachSelectedTile - %s", m_name.ToCStr()));

	int seed = generator.GetSeed() + m_params.m_seedOffset;

	m_cachedNoiseValues.resize(world.m_tiles.GetSize(), 0.f);

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		int tileIndex = world.m_tiles.GetIndexForCoords(tileCoords);
		float noiseValue = Noise::GetPerlinNoise2D((float) tileCoords.x, (float) tileCoords.y, m_params.m_scale, m_params.m_numOctaves, m_params.m_octavePersistence, m_params.m_octaveScale, m_params.m_renormalize, seed);
		float rangeMappedNoise = MathUtils::RangeMapClamped(noiseValue, -1.f, 1.f, m_params.m_outputRange.x, m_params.m_outputRange.y);
		m_cachedNoiseValues[tileIndex] = rangeMappedNoise;
		return true; // Continue iterating
	});

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		if (!world.DoesTileMatchTagQuery(tileCoords, m_tagQuery))
		{
			return true;
		}

		int tileIndex = world.m_tiles.GetIndexForCoords(tileCoords);
		float const& noiseValue = m_cachedNoiseValues[tileIndex];

		bool isNoiseValuePeak = true;
		world.ForEachPlayableNeighboringTile(tileCoords, [&](IntVec2 const& neighborCoords)
		{
			int neighborIndex = world.m_tiles.GetIndexForCoords(neighborCoords);
			float const& neighborNoiseValue = m_cachedNoiseValues[neighborIndex];
			if (noiseValue <= neighborNoiseValue)
			{
				// One neighbor is larger, so this is not a peak
				isNoiseValuePeak = false;
				return false;
			}
			return true; // Continue checking neighbors
		});

		if (isNoiseValuePeak)
		{
			if (!func(tileCoords))
			{
				return false; // Stop iterating
			}
		}
		return true; // Continue iterating
	});

	m_cachedNoiseValues.clear();
}



//----------------------------------------------------------------------------------------------------------------------
TileGeneratorComponent::TileGeneratorComponent(TileGeneratorComponentDef const& def) : m_tileSelectorName(def.m_tileSelectorName), m_tileName(def.m_tileName)
{

}




//----------------------------------------------------------------------------------------------------------------------
bool TileGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	ScopedTimer timer("TileGeneratorComponent::Generate");

	TileDef const* tileDef = TileDef::GetTileDef(m_tileName);
	if (!tileDef)
	{
		ERROR_AND_DIE(StringUtils::StringF("TileDef not found for tile: %s", m_tileName.ToCStr()));
	}

	Tile defaultTile = tileDef->GetDefaultTile(m_tileName);

	TileSelectorComponent* selector = generator.GetTileSelectorComponentByName(m_tileSelectorName);
	if (!selector)
	{
		ERROR_AND_DIE(StringUtils::StringF("TileSelectorComponent not found: %s", m_tileSelectorName.ToCStr()));
	}

	selector->ForEachSelectedTile(generator, world, [&](IntVec2 const& tileCoords)
	{
		world.SetTile(tileCoords, defaultTile);
		return true; // Continue iterating
	});

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
EntityGeneratorComponent::EntityGeneratorComponent(EntityGeneratorComponentDef const& def) : m_tileSelectorName(def.m_tileSelectorName), m_entityName(def.m_entityName)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool EntityGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	ScopedTimer timer("EntityGeneratorComponent::Generate");

	EntityDef const* entityDef = EntityDef::GetEntityDef(m_entityName);
	if (!entityDef)
	{
		ERROR_AND_DIE(StringUtils::StringF("EntityDef not found for entity: %s", m_entityName.ToCStr()));
	}

	TileSelectorComponent* selector = generator.GetTileSelectorComponentByName(m_tileSelectorName);
	if (!selector)
	{
		ERROR_AND_DIE(StringUtils::StringF("TileSelectorComponent not found: %s", m_tileSelectorName.ToCStr()));
	}


	selector->ForEachSelectedTile(generator, world, [&](IntVec2 const& tileCoords)
	{
		TowerPlacementInfo placementInfo = SInput::MakeTowerPlacementInfo(m_entityName, world.GetTileCenter(tileCoords), world);

		world.m_generatedTowers.push_back(placementInfo);
		return true; // Continue iterating
	});

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
DiscGoalGeneratorComponent::DiscGoalGeneratorComponent(DiscGoalGeneratorComponentDef const& def) : m_alignment(def.m_alignment), m_radius(def.m_radius)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool DiscGoalGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	ScopedTimer timer("DiscGoalGeneratorComponent::Generate");

	float goalCenterX = StaticWorldSettings::s_visibleWorldMinsX + (StaticWorldSettings::s_visibleWorldWidth * m_alignment.x);
	float goalCenterY = StaticWorldSettings::s_visibleWorldMinsY + (StaticWorldSettings::s_visibleWorldHeight * m_alignment.y);

	float goalRadius = m_radius;
	goalRadius *= 0.99f;

	Vec2 goalCenter = Vec2(goalCenterX, goalCenterY);

	BiomeDef const* biomeDef = generator.GetBiomeDef();
	Name tileName = biomeDef->m_goalTile;

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
RectGoalGeneratorComponent::RectGoalGeneratorComponent(RectGoalGeneratorComponentDef const& def) : m_alignment(def.m_alignment), m_dims(def.m_dims)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool RectGoalGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	ScopedTimer timer("RectGoalGeneratorComponent::Generate");

	float goalCenterX = StaticWorldSettings::s_visibleWorldMinsX + (StaticWorldSettings::s_visibleWorldWidth * m_alignment.x);
	float goalCenterY = StaticWorldSettings::s_visibleWorldMinsY + (StaticWorldSettings::s_visibleWorldHeight * m_alignment.y);

	Vec2 goalCenter = Vec2(goalCenterX, goalCenterY);
	AABB2 goalBounds = AABB2(goalCenter, (float) m_dims.x * 0.5f, (float) m_dims.y * 0.5f);
	goalBounds.Squeeze(0.01f); // Make sure we don't count tiles past the end of the AABB if it lines up perfectly with tile edges

	BiomeDef const* biomeDef = generator.GetBiomeDef();
	Name tileName = biomeDef->m_goalTile;

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
PerlinWormPathGeneratorComponent::PerlinWormPathGeneratorComponent(PerlinWormPathGeneratorComponentDef const& def)
{
	m_startPos = def.m_startPos;
	m_startDir = def.m_startDir;
	m_thicknessRange = def.m_thicknessRange;
	m_thicknessVariance = def.m_thicknessVariance;
	m_splitChance = def.m_splitChance;
	m_splitAngleDeg = def.m_splitAngleDeg;
	m_maxSplits = def.m_maxSplits;
	m_seedOffset = def.m_seedOffset;
}



//----------------------------------------------------------------------------------------------------------------------
bool PerlinWormPathGeneratorComponent::Generate(MapGenerator& generator, SCWorld& world)
{
	ScopedTimer timer("PerlinWormPathGeneratorComponent::Generate");

	struct PerlinWorm
	{
		Vec2 m_pos;
		Vec2 m_dir;
		IntVec2 m_tileCoords;
	};

	int seed = generator.GetSeed() + m_seedOffset;

	float startX = StaticWorldSettings::s_visibleWorldMinsX + (StaticWorldSettings::s_visibleWorldWidth * m_startPos.x);
	float startY = StaticWorldSettings::s_visibleWorldMinsY + (StaticWorldSettings::s_visibleWorldHeight * m_startPos.y);
	Vec2 startPos = Vec2(startX, startY);

	BiomeDef const* biomeDef = generator.GetBiomeDef();
	Tile pathTile = TileDef::GetDefaultTile(biomeDef->m_pathTile);
	pathTile.SetIsPath(true);

	std::vector<PerlinWorm> worms;
	worms.emplace_back(PerlinWorm{ startPos, m_startDir, world.GetTileCoordsAtWorldPos(startPos) });

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

			bool canSplit = numSplits < m_maxSplits;
			if (canSplit)
			{
				float splitNoise = Noise::GetNoiseZeroToOne1D(static_cast<int>(noiseLocation) + 1000, seed + iterationCount);
				bool didSplit = splitNoise < m_splitChance;
				if (didSplit)
				{
					numSplits++;
					PerlinWorm splitWorm = worm;
					float splitRotation = iterationCount % 2 == 0 ? m_splitAngleDeg : -m_splitAngleDeg;
					splitWorm.m_dir = worm.m_dir.GetRotated(splitRotation);
					worms.push_back(splitWorm);
				}
			}

			float capsuleRadius = m_thicknessRange.x * 0.5f;
			float capsuleVariance = m_thicknessRange.y - m_thicknessRange.x;
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
