// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/Name.h"
#include <vector>



struct MapGeneratorComponentDef;



//----------------------------------------------------------------------------------------------------------------------
struct BiomeGeneratorDef
{
public:

	explicit BiomeGeneratorDef(XmlElement const* biomeGeneratorDefXmlElement);
	~BiomeGeneratorDef();

	static void LoadFromXML();
	static void Shutdown();
	static BiomeGeneratorDef const* GetBiomeGeneratorDef(Name name);

private:

	static std::vector<BiomeGeneratorDef> s_biomeGeneratorDefs;

public:

	Name		m_name			= "Unnamed BiomeGeneratorDef";
	Name		m_baseTile		= "grass";
	std::vector<MapGeneratorComponentDef const*> m_generatorComponentDefs;
};