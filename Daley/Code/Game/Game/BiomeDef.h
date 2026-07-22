// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/Name.h"
#include <vector>



struct MapGeneratorComponentDef;



//----------------------------------------------------------------------------------------------------------------------
struct BiomeDef
{
public:

	explicit BiomeDef(XmlElement const* biomeDefXmlElement);
	~BiomeDef();

	static void LoadFromXML();
	static void Shutdown();
	static BiomeDef const* GetBiomeDef(Name name);

private:

	static std::vector<BiomeDef> s_biomeDefs;

public:

	Name		m_name			= "Unnamed BiomeDef";
	Name		m_baseTile		= "grass";
	Name		m_goalTile		= "islandWater";
	Name 		m_pathTile		= "snow";
	std::vector<MapGeneratorComponentDef const*> m_generatorComponentDefs;
};