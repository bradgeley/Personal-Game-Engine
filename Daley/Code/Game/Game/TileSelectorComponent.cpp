// Bradley Christensen - 2022-2026
#include "TileSelectorComponent.h"
#include "MapGenerator.h"
#include "TileSelectorComponentDef.h"
#include "SCWorld.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Performance/ScopedTimer.h"
#include <queue>



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent::TileSelectorComponent(TileSelectorComponentDef const& def) : m_def(&def)
{

}



//----------------------------------------------------------------------------------------------------------------------
NoiseRangeSelectorComponent::NoiseRangeSelectorComponent(NoiseRangeSelectorComponentDef const& def) : TileSelectorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void NoiseRangeSelectorComponent::ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func)
{
	NoiseRangeSelectorComponentDef const& def = GetDef();

	ScopedTimer timer(StringUtils::StringF("NoiseRangeSelectorComponent::ForEachSelectedTile - %s", def.m_name.ToCStr()));

	NoiseParams const& params = def.m_noiseParams;

	uint32_t seed = generator.GetSeed() + static_cast<uint32_t>(params.m_seedOffset);

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		if (!world.DoesTileMatchTagQuery(tileCoords, def.m_tagQuery))
		{
			return true;
		}

		if (def.m_chanceToSelect < 1.f)
		{
			float randomValue = Noise::GetNoiseZeroToOne2D(tileCoords.x, tileCoords.y, generator.GetSeed() + 1);
			if (randomValue > def.m_chanceToSelect)
			{
				return true; // Skip this tile, deterministically
			}
		}

		float noiseValue = Noise::GetPerlinNoise2D((float) tileCoords.x, (float) tileCoords.y, params.m_scale, params.m_numOctaves, params.m_octavePersistence, params.m_octaveScale, params.m_renormalize, seed);
		float rangeMappedNoise = MathUtils::RangeMapClamped(noiseValue, -1.f, 1.f, params.m_outputRange.x, params.m_outputRange.y);

		if (rangeMappedNoise >= def.m_noiseRange.x && rangeMappedNoise <= def.m_noiseRange.y)
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
NoiseRangeSelectorComponentDef const& NoiseRangeSelectorComponent::GetDef() const
{
	return *static_cast<NoiseRangeSelectorComponentDef const*>(m_def);
}



//----------------------------------------------------------------------------------------------------------------------
NoisePeakSelectorComponent::NoisePeakSelectorComponent(NoisePeakSelectorComponentDef const& def) : TileSelectorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void NoisePeakSelectorComponent::ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func)
{
	NoisePeakSelectorComponentDef const& def = GetDef();

	ScopedTimer timer(StringUtils::StringF("NoisePeakSelectorComponent::ForEachSelectedTile - %s", m_def->m_name.ToCStr()));

	NoiseParams const& params = def.m_noiseParams;
	uint32_t seed = generator.GetSeed() + static_cast<uint32_t>(params.m_seedOffset);

	m_cachedNoiseValues.resize(world.m_tiles.GetSize(), 0.f);

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		int tileIndex = world.m_tiles.GetIndexForCoords(tileCoords);
		float noiseValue = Noise::GetPerlinNoise2D((float) tileCoords.x, (float) tileCoords.y, params.m_scale, params.m_numOctaves, params.m_octavePersistence, params.m_octaveScale, params.m_renormalize, seed);
		float rangeMappedNoise = MathUtils::RangeMapClamped(noiseValue, -1.f, 1.f, params.m_outputRange.x, params.m_outputRange.y);
		m_cachedNoiseValues[tileIndex] = rangeMappedNoise;
		return true; // Continue iterating
	});

	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		if (!world.DoesTileMatchTagQuery(tileCoords, def.m_tagQuery))
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
NoisePeakSelectorComponentDef const& NoisePeakSelectorComponent::GetDef() const
{
	return *static_cast<NoisePeakSelectorComponentDef const*>(m_def);
}



//----------------------------------------------------------------------------------------------------------------------
DistanceFieldSelectorComponent::DistanceFieldSelectorComponent(DistanceFieldSelectorComponentDef const& def) : TileSelectorComponent(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void DistanceFieldSelectorComponent::ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func)
{
	DistanceFieldSelectorComponentDef const& def = GetDef();

	m_distanceFieldValues.clear();

	m_distanceFieldValues.resize(world.m_tiles.GetSize(), 9999);

	std::priority_queue<IntVec2> openList;

	// Seed tiles
	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		if (!world.DoesTileMatchTagQuery(tileCoords, def.m_tagQuery))
		{
			return true;
		}

		int tileIndex = world.m_tiles.GetIndexForCoords(tileCoords);
		m_distanceFieldValues[tileIndex] = 0;
		openList.emplace(IntVec2(0, tileIndex));
		return true; // Continue iterating
	});

	// Run Dijkstra's algorithm to compute distance field
	while (openList.empty() == false)
	{
		IntVec2 pair = openList.top();
		openList.pop();
		int const& currentDistance = pair.x;
		int const& currentIndex = pair.y;

		if (currentDistance > m_distanceFieldValues[currentIndex])
		{
			continue; // Already found a shorter path to this tile
		}

		IntVec2 currentCoords = world.m_tiles.GetCoordsForIndex(currentIndex);
		world.ForEachPlayableNeighboringTile(currentCoords, [&](IntVec2 const& neighborCoords)
		{
			int neighborIndex = world.m_tiles.GetIndexForCoords(neighborCoords);
			int newDistance = currentDistance + 1; // Assuming uniform cost for each step
			if (newDistance < m_distanceFieldValues[neighborIndex])
			{
				m_distanceFieldValues[neighborIndex] = newDistance;
				openList.emplace(IntVec2(newDistance, neighborIndex));
			}
			return true; // Continue checking neighbors
		});
	}

	// Run selector logic
	world.ForEachPlayableTile([&](IntVec2 const& tileCoords)
	{
		int tileIndex = world.m_tiles.GetIndexForCoords(tileCoords);
		int distanceValue = m_distanceFieldValues[tileIndex];
		if (distanceValue >= def.m_distanceRange.x && distanceValue <= def.m_distanceRange.y)
		{
			if (def.m_chanceToSelect < 1.f)
			{
				float randomValue = Noise::GetNoiseZeroToOne2D(tileCoords.x, tileCoords.y, generator.GetSeed() + 1);
				if (randomValue > def.m_chanceToSelect)
				{
					return true; // Skip this tile, deterministically
				}
			}

			if (!func(tileCoords))
			{
				return false; // Stop iterating
			}
		}
		return true; // Continue iterating
	});
}



//----------------------------------------------------------------------------------------------------------------------
DistanceFieldSelectorComponentDef const& DistanceFieldSelectorComponent::GetDef() const
{
	return *static_cast<DistanceFieldSelectorComponentDef const*>(m_def);
}
