// Bradley Christensen - 2022-2026
#pragma once
#include <vector>



class MapGeneratorComponent;
class SCWorld;
class TileSelectorComponent;
struct BiomeDef;
struct MapGeneratorDef;
struct Name;



//----------------------------------------------------------------------------------------------------------------------
class MapGenerator
{
public:

	MapGenerator() = default;
	MapGenerator(MapGenerator const&) = delete;
	MapGenerator& operator=(MapGenerator const&) = delete;
	~MapGenerator();

	void Initialize(MapGeneratorDef const& mapGenDef, int seed);
	int GetSeed() const { return m_seed; }
	BiomeDef const* GetBiomeDef() const;
	TileSelectorComponent* GetTileSelectorComponentByName(Name const& name) const;

	bool GenerateMap(SCWorld& world);

private:

	MapGeneratorDef const* m_def = nullptr;
	int m_seed = 0;
	std::vector<MapGeneratorComponent*> m_components;
	//std::vector<MapGeneratorModifier> m_modifiers;
};