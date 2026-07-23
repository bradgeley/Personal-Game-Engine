// Bradley Christensen - 2022-2026
#include "MapGenerator.h"
#include "BiomeDef.h"
#include "MapGeneratorDef.h"
#include "MapGeneratorComponent.h"
#include "MapGeneratorComponentDef.h"
#include "SCWorld.h"
#include "Tile.h"
#include "TileDef.h"
#include "WorldSettings.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
MapGenerator::~MapGenerator()
{
	for (MapGeneratorComponent* component : m_components)
	{
		delete component;
	}
	m_components.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void MapGenerator::Initialize(MapGeneratorDef const& mapGenDef, int seed)
{
	m_def = &mapGenDef;
	m_seed = seed;

	BiomeDef const* biomeDef = BiomeDef::GetBiomeDef(mapGenDef.m_biome);
	if (biomeDef)
	{
		for (MapGeneratorComponentDef const* componentDef : biomeDef->m_generatorComponentDefs)
		{
			MapGeneratorComponent* component = componentDef->MakeComponentInstance();
			m_components.push_back(component);
		}
	}

	for (MapGeneratorComponentDef const* componentDef : m_def->m_mapGeneratorComponentDefs)
	{
		MapGeneratorComponent* component = componentDef->MakeComponentInstance();
		m_components.push_back(component);
	}
}



//----------------------------------------------------------------------------------------------------------------------
BiomeDef const* MapGenerator::GetBiomeDef() const
{
	return BiomeDef::GetBiomeDef(m_def->m_biome);
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent* MapGenerator::GetTileSelectorComponentByName(Name const& name) const
{
	for (MapGeneratorComponent* component : m_components)
	{
		TileSelectorComponent* tileSelector = dynamic_cast<TileSelectorComponent*>(component);
		if (tileSelector && tileSelector->m_name == name)
		{
			return tileSelector;
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool MapGenerator::GenerateMap(SCWorld& world)
{
	ScopedTimer timer("Generate Map");

	world.m_generatedTowers.clear();

	ASSERT_OR_DIE(m_def != nullptr, "MapGenerator::GenerateMap() - m_def is null. Did you call Initialize()?");
	BiomeDef const* biomeDef = BiomeDef::GetBiomeDef(m_def->m_biome);
	ASSERT_OR_DIE(biomeDef, StringUtils::StringF("Failed to find biome def for biome: %s", m_def->m_biome.ToCStr()));

	Tile backgroundTile = TileDef::GetDefaultTile(biomeDef->m_baseTile);
	world.m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), backgroundTile);

	for (MapGeneratorComponent* component : m_components)
	{
		component->Generate(*this, world);
	}

	// Validate Map

	// Cache spawn locations (for perf)
	world.CacheValidSpawnLocations();

	return true;
}
