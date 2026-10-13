// Bradley Christensen - 2022-2026
#include "MapGenerator.h"
#include "BiomeDef.h"
#include "MapGeneratorComponent.h"
#include "MapGeneratorComponentDef.h"
#include "MapGeneratorDef.h"
#include "SCWorld.h"
#include "Tile.h"
#include "TileDef.h"
#include "TileSelectorComponent.h"
#include "TileSelectorComponentDef.h"
#include "WorldSettings.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Performance/ScopedTimer.h"
#include <algorithm>



//----------------------------------------------------------------------------------------------------------------------
MapGenerator::~MapGenerator()
{
	for (MapGeneratorComponent* component : m_components)
	{
		delete component;
	}
	m_components.clear();

	for (TileSelectorComponent* selector : m_tileSelectors)
	{
		delete selector;
	}
	m_tileSelectors.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void MapGenerator::Initialize(MapGeneratorDef const& mapGenDef, BiomeDef const& biomeDef, uint32_t seed)
{
	m_def = &mapGenDef;
	m_biomeDef = &biomeDef;
	m_seed = seed;

	for (MapGeneratorComponentDef const* componentDef : m_biomeDef->m_generatorComponentDefs)
	{
		MapGeneratorComponent* component = componentDef->MakeComponentInstance();
		m_components.push_back(component);
	}

	for (TileSelectorComponentDef const* selectorDef : m_biomeDef->m_tileSelectorComponentDefs)
	{
		TileSelectorComponent* selector = selectorDef->MakeComponentInstance();
		m_tileSelectors.push_back(selector);
	}

	for (MapGeneratorComponentDef const* componentDef : m_def->m_mapGeneratorComponentDefs)
	{
		MapGeneratorComponent* component = componentDef->MakeComponentInstance();
		m_components.push_back(component);
	}

	for (TileSelectorComponentDef const* selectorDef : m_def->m_tileSelectorComponentDefs)
	{
		TileSelectorComponent* selector = selectorDef->MakeComponentInstance();
		m_tileSelectors.push_back(selector);
	}

	std::sort(m_components.begin(), m_components.end(), [](MapGeneratorComponent* a, MapGeneratorComponent* b) { return a->m_runOrder < b->m_runOrder; });
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent* MapGenerator::GetTileSelectorComponentByName(Name const& name) const
{
	for (TileSelectorComponent* component : m_tileSelectors)
	{
		if (component && component->m_def->m_name == name)
		{
			return component;
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool MapGenerator::GenerateMap(SCWorld& world)
{
	ScopedTimer timer("Generate Map");

	world.m_mapDefName = m_def->m_name;

	world.m_generatedTowers.clear();

	ASSERT_OR_DIE(m_def != nullptr, "MapGenerator::GenerateMap() - m_def is null. Did you call Initialize()?");
	ASSERT_OR_DIE(m_biomeDef, StringUtils::StringF("Failed to find biome def for biome: %s", m_biomeDef->m_name.ToCStr()));

	Tile backgroundTile = TileDef::GetDefaultTile(m_biomeDef->m_baseTile);
	world.m_tiles.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), backgroundTile);
	world.m_numEnemiesInTile.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), 0);
	world.m_tileOccupancy.Initialize(IntVec2(StaticWorldSettings::s_numTilesInRow, StaticWorldSettings::s_numTilesInRow), EntityID::Invalid);

	for (MapGeneratorComponent* component : m_components)
	{
		component->Generate(*this, world);
	}

	// Cache Data
	world.OnMapGenerationComplete();

	// Validate Map

	return true;
}
