// Bradley Christensen - 2022-2026
#pragma once
#include <vector>



class SCWorld;
struct MapGeneratorDef;



//----------------------------------------------------------------------------------------------------------------------
class MapGenerator
{
public:

	MapGenerator(MapGeneratorDef const& mapGenDef, int seed, SCWorld& world);

	bool GenerateMap() const;

private:

	MapGeneratorDef const* m_def = nullptr;
	int m_seed = 0;
	SCWorld* m_world = nullptr;
	//std::vector<MapGeneratorModifier> m_modifiers;
};