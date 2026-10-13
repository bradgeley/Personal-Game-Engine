// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/Name.h"
#include <vector>



struct MapGeneratorComponentDef;
struct TileSelectorComponentDef;



//----------------------------------------------------------------------------------------------------------------------
struct MapGeneratorDef
{
public:

    MapGeneratorDef(XmlElement const* mapGeneratorDefXmlElement);
    ~MapGeneratorDef();

    // Delete copy operations
    MapGeneratorDef(MapGeneratorDef const&) = delete;
    MapGeneratorDef& operator=(MapGeneratorDef const&) = delete;

    // Implement move operations
    MapGeneratorDef(MapGeneratorDef&& other) noexcept;
    MapGeneratorDef& operator=(MapGeneratorDef&& other) noexcept;

	static void LoadFromXML();
	static void Shutdown();
	static MapGeneratorDef const* GetMapGeneratorDef(Name name);
	static MapGeneratorDef const* GetRandom(int seed);

private:

	static std::vector<MapGeneratorDef> s_mapGeneratorDefs;

public:

	Name m_name = "Unnamed MapGeneratorDef";
	 
	std::vector<MapGeneratorComponentDef*> m_mapGeneratorComponentDefs;
	std::vector<TileSelectorComponentDef*> m_tileSelectorComponentDefs;
}; 