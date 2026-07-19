// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/Name.h"
#include <vector>



struct MapGeneratorComponentDef;



//----------------------------------------------------------------------------------------------------------------------
struct MapGeneratorDef
{
public:

	explicit MapGeneratorDef(XmlElement const* mapGeneratorDefXmlElement);
	~MapGeneratorDef();

	static void LoadFromXML();
	static void Shutdown();
	static MapGeneratorDef const* GetMapGeneratorDef(Name name);

private:

	static std::vector<MapGeneratorDef> s_mapGeneratorDefs;

public:

	Name m_name = "Unnamed MapGeneratorDef";
	 
	std::vector<MapGeneratorComponentDef*> m_mapGeneratorComponentDefs;
}; 