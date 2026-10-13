// Bradley Christensen - 2022-2026
#pragma once
#include "MapGeneratorComponentDef.h"
#include "Engine/Core/Name.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



class MapGenerator;
class SCWorld;



//----------------------------------------------------------------------------------------------------------------------
class MapGeneratorComponent
{
public:

	virtual ~MapGeneratorComponent() = default;

	virtual bool Generate(MapGenerator& generator, SCWorld& world) = 0;

	int m_runOrder = 0;
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

	Vec2	 m_startPos = Vec2(0.f, 0.f); // 0-1 alignment of the start pos on the map
	Vec2	 m_startDir = Vec2(1.f, 0.f);
	Vec2	 m_thicknessRange = Vec2(2.f, 5.f);
	float	 m_thicknessNoiseScale = 0.5f;
	float	 m_splitChance = 0.0075f;
	float	 m_splitAngleDeg = 35.f;
	int		 m_maxSplits = 4;
	uint32_t m_seedOffset = 0;
};