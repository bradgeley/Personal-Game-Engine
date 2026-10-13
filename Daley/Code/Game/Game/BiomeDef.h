// Bradley Christensen - 2022-2026
#pragma once
#include "SCWaves.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/Name.h"
#include <vector>



struct MapGeneratorComponentDef;
struct TileSelectorComponentDef;



//----------------------------------------------------------------------------------------------------------------------
struct BiomeDef
{
public:

	explicit BiomeDef(XmlElement const* biomeDefXmlElement);
	explicit BiomeDef(BiomeDef const& other) = delete;

	void Release();

	static void LoadFromXML();
	static void Shutdown();
	static BiomeDef const* GetBiomeDef(Name name);

	void operator=(BiomeDef const& other) = delete;

private:

	static std::vector<BiomeDef*> s_biomeDefs;

public:

	Name		m_name			= "Unnamed BiomeDef";
	Name		m_baseTile		= "grass";
	Name		m_goalTile		= "islandWater";
	Name 		m_pathTile		= "snow";
	std::vector<MapGeneratorComponentDef const*> m_generatorComponentDefs;
	std::vector<TileSelectorComponentDef const*> m_tileSelectorComponentDefs;
	LevelWaveGenDef m_waveGenDef;
};