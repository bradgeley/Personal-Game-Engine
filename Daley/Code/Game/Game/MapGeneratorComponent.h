// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"
#include <vector>



class MapGenerator;
class SCWorld;
struct MapGeneratorComponentDef;



//----------------------------------------------------------------------------------------------------------------------
class MapGeneratorComponent
{
public:

	MapGeneratorComponent(MapGeneratorComponentDef const& def);
	virtual ~MapGeneratorComponent() = default;

	virtual bool Generate(MapGenerator& generator, SCWorld& world) = 0;

	MapGeneratorComponentDef const* m_def = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class TileSelectorComponent : public MapGeneratorComponent
{
public:

	TileSelectorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
	virtual bool SelectTiles(MapGenerator& generator, SCWorld& world) = 0;

public:

	std::vector<IntVec2> m_selectedTiles;
};



//----------------------------------------------------------------------------------------------------------------------
class NoiseSelectorComponent : public TileSelectorComponent
{
public:

	NoiseSelectorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
	virtual bool SelectTiles(MapGenerator& generator, SCWorld& world) override = 0;
};



//----------------------------------------------------------------------------------------------------------------------
class NoiseRangeSelectorComponent : public NoiseSelectorComponent
{
public:

	NoiseRangeSelectorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
	virtual bool SelectTiles(MapGenerator& generator, SCWorld& world) override;
};



//----------------------------------------------------------------------------------------------------------------------
class NoisePeakSelectorComponent : public NoiseSelectorComponent
{
public:

	NoisePeakSelectorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
	virtual bool SelectTiles(MapGenerator& generator, SCWorld& world) override;
};



//----------------------------------------------------------------------------------------------------------------------
class BiomeGeneratorComponent : public MapGeneratorComponent
{
public:

	BiomeGeneratorComponent(MapGeneratorComponentDef const& def);
	~BiomeGeneratorComponent();

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;

public:

	std::vector<MapGeneratorComponent*> m_biomeComponents;
};



//----------------------------------------------------------------------------------------------------------------------
class DiscreteGeneratorComponent : public MapGeneratorComponent
{
public:

	DiscreteGeneratorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
};



//----------------------------------------------------------------------------------------------------------------------
class TileGeneratorComponent : public DiscreteGeneratorComponent
{
public:

	TileGeneratorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
};



//----------------------------------------------------------------------------------------------------------------------
class EntityGeneratorComponent : public DiscreteGeneratorComponent
{
public:

	EntityGeneratorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
};


//----------------------------------------------------------------------------------------------------------------------
class GoalGeneratorComponent : public MapGeneratorComponent
{
public:

	GoalGeneratorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
};



//----------------------------------------------------------------------------------------------------------------------
class DiscGoalGeneratorComponent : public GoalGeneratorComponent
{
public:

	DiscGoalGeneratorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
};



//----------------------------------------------------------------------------------------------------------------------
class RectGoalGeneratorComponent : public GoalGeneratorComponent
{
public:

	RectGoalGeneratorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
};



//----------------------------------------------------------------------------------------------------------------------
class PerlinWormPathGeneratorComponent : public MapGeneratorComponent
{
public:

	PerlinWormPathGeneratorComponent(MapGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override;
};