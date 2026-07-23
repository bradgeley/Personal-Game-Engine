// Bradley Christensen - 2022-2026
#pragma once
#include "MapGeneratorComponentDef.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/TagQuery.h"
#include "Engine/Core/XmlUtils.h"
#include <vector>



class MapGenerator;
class SCWorld;



//----------------------------------------------------------------------------------------------------------------------
class MapGeneratorComponent
{
public:

	virtual ~MapGeneratorComponent() = default;

	virtual bool Generate(MapGenerator& generator, SCWorld& world) = 0;
};



//----------------------------------------------------------------------------------------------------------------------
class TileSelectorComponent : public MapGeneratorComponent
{
public:

	virtual void ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func) = 0;
	virtual bool Generate(MapGenerator&, SCWorld&) override final { return true; } // todo: separate out Selectors from Generators, so this func override goes away

public:

	Name m_name = "Unnamed TileSelectorDef";
};



//----------------------------------------------------------------------------------------------------------------------
class NoiseRangeSelectorComponent : public TileSelectorComponent
{
public:

	NoiseRangeSelectorComponent(NoiseRangeSelectorComponentDef const& def);

	virtual void ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func) override final;

public:

	Vec2 m_noiseRange = Vec2::ZeroVector;
	NoiseParams m_params;
	TagQuery m_tagQuery;
};



//----------------------------------------------------------------------------------------------------------------------
class NoisePeakSelectorComponent : public TileSelectorComponent
{
public:

	NoisePeakSelectorComponent(NoisePeakSelectorComponentDef const& def);

	virtual void ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func) override final;

public:

	NoiseParams m_params;
	TagQuery m_tagQuery;
	std::vector<float> m_cachedNoiseValues;
};



//----------------------------------------------------------------------------------------------------------------------
class TileGeneratorComponent : public MapGeneratorComponent
{
public:

	TileGeneratorComponent(TileGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override final;

public:

	Name m_tileSelectorName;
	Name m_tileName = "grass";
};



//----------------------------------------------------------------------------------------------------------------------
class EntityGeneratorComponent : public MapGeneratorComponent
{
public:

	EntityGeneratorComponent(EntityGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override final;

public:

	Name m_tileSelectorName;
	Name m_entityName = "wall1x1";
};



//----------------------------------------------------------------------------------------------------------------------
class DiscGoalGeneratorComponent : public MapGeneratorComponent
{
public:

	DiscGoalGeneratorComponent(DiscGoalGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override final;

public:

	Vec2 m_alignment = Vec2(0.5f, 0.5f);
	float m_radius = 4.f;
};



//----------------------------------------------------------------------------------------------------------------------
class RectGoalGeneratorComponent : public MapGeneratorComponent
{
public:

	RectGoalGeneratorComponent(RectGoalGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override final;

public:

	Vec2 m_alignment = Vec2(0.5f, 0.5f);
	IntVec2 m_dims = IntVec2(4, 4);
};



//----------------------------------------------------------------------------------------------------------------------
class PerlinWormPathGeneratorComponent : public MapGeneratorComponent
{
public:

	PerlinWormPathGeneratorComponent(PerlinWormPathGeneratorComponentDef const& def);

	virtual bool Generate(MapGenerator& generator, SCWorld& world) override final;

public:

	Vec2	m_startPos			= Vec2(0.f, 0.f); // 0-1 alignment of the start pos on the map
	Vec2	m_startDir			= Vec2(1.f, 0.f);
	Vec2	m_thicknessRange	= Vec2(2.f, 5.f);
	float	m_thicknessVariance = 0.5f;
	float	m_splitChance		= 0.0075f;
	float	m_splitAngleDeg		= 35.f;
	int		m_maxSplits			= 4;
	int		m_seedOffset		= 0;
};