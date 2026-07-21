// Bradley Christensen - 2022-2026
#include "MapGenerator.h"
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

	for (MapGeneratorComponentDef const* componentDef : m_def->m_mapGeneratorComponentDefs)
	{
		MapGeneratorComponent* component = componentDef->MakeComponentInstance();
		m_components.push_back(component);
	}
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent* MapGenerator::GetTileSelectorComponentByName(Name const& name) const
{
	for (MapGeneratorComponent* component : m_components)
	{
		TileSelectorComponent* tileSelector = dynamic_cast<TileSelectorComponent*>(component);
		if (tileSelector)
		{
			TileSelectorComponentDef const* def = dynamic_cast<TileSelectorComponentDef const*>(tileSelector->m_def);
			if (def && def->m_name == name)
			{
				return tileSelector;
			}
		}

		BiomeGeneratorComponent* biomeGenerator = dynamic_cast<BiomeGeneratorComponent*>(component);
		if (biomeGenerator)
		{
			for (MapGeneratorComponent* biomeComponent : biomeGenerator->m_biomeComponents)
			{
				tileSelector = dynamic_cast<TileSelectorComponent*>(biomeComponent);
				if (tileSelector)
				{
					TileSelectorComponentDef const* def = dynamic_cast<TileSelectorComponentDef const*>(tileSelector->m_def);
					if (def && def->m_name == name)
					{
						return tileSelector;
					}
				}
			}
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
bool MapGenerator::GenerateMap(SCWorld& world)
{
	ScopedTimer timer("Generate Map");

	for (MapGeneratorComponent* component : m_components)
	{
		component->Generate(*this, world);
	}

	// Validate Map

	// Cache spawn locations (for perf)
	world.CacheValidSpawnLocations();

	return true;
}
