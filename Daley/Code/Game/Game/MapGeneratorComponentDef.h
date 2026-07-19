// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
struct NoiseParams
{
	NoiseParams() = default;
	NoiseParams(XmlElement const* xmlElement);

	Vec2	m_outputRange = Vec2(0.f, 1.f); // Result will be range mapped to this range after renormalization
	float	m_scale = 1.f;
	int		m_numOctaves = 1;
	float	m_octavePersistence = 0.5f;
	float	m_octaveScale = 2.f;
	bool	m_renormalize = true;
	int		m_seedOffset = 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct MapGeneratorComponentDef
{
	MapGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~MapGeneratorComponentDef() = default;

	// Factory function to turn a generic componnet xml Element into an allocated def
	static MapGeneratorComponentDef const* MakeFromXmlElement(XmlElement const* xmlElement);
};



//----------------------------------------------------------------------------------------------------------------------
struct TileSelectorDef : public MapGeneratorComponentDef
{
	TileSelectorDef(XmlElement const* xmlElement);

	Name m_name = "Unnamed TileSelectorDef";
};



//----------------------------------------------------------------------------------------------------------------------
struct NoiseSelectorDef : public TileSelectorDef
{
	NoiseSelectorDef(XmlElement const* xmlElement);
	~NoiseSelectorDef() = default;

	NoiseParams m_noiseParams;
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles in a range of noise values
//
struct NoiseRangeSelectorDef : public NoiseSelectorDef
{
	NoiseRangeSelectorDef(XmlElement const* xmlElement);
	~NoiseRangeSelectorDef() = default;

	Vec2 m_noiseRange = Vec2(0.f, 1.f);
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles at local maxima of noise values
//
struct NoisePeakSelectorDef : public NoiseSelectorDef
{
	NoisePeakSelectorDef(XmlElement const* xmlElement);
	~NoisePeakSelectorDef() = default;
};



//----------------------------------------------------------------------------------------------------------------------
struct BiomeGeneratorComponentDef : public MapGeneratorComponentDef
{
	BiomeGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~BiomeGeneratorComponentDef() = default;

	Name m_biomeName = "forest"; // Corresponds to a BiomeDef used to generate tiles for this map
};



//----------------------------------------------------------------------------------------------------------------------
// Generators that use a discrete tile selector (one that returns a tile set to operate on)
//
struct DiscreteGeneratorComponentDef : public MapGeneratorComponentDef
{
	DiscreteGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~DiscreteGeneratorComponentDef() = default;

	Name m_tileSelectorName;
};



//----------------------------------------------------------------------------------------------------------------------
struct TileGeneratorComponentDef : public DiscreteGeneratorComponentDef
{
	TileGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~TileGeneratorComponentDef() = default;

	Name m_tileName = "grass";
};



//----------------------------------------------------------------------------------------------------------------------
struct EntityGeneratorComponentDef : public DiscreteGeneratorComponentDef
{
	EntityGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~EntityGeneratorComponentDef() = default;

	Name m_entityName = "grass";
};


//----------------------------------------------------------------------------------------------------------------------
struct GoalGeneratorDef : public MapGeneratorComponentDef
{
	GoalGeneratorDef(XmlElement const* xmlElement);
	virtual ~GoalGeneratorDef() = default;

	Vec2 m_mapAlignment = Vec2(0.5f, 0.5f); // 0 = left/bottom, 1 = right/top
};



//----------------------------------------------------------------------------------------------------------------------
struct DiscGoalGeneratorDef : public GoalGeneratorDef
{
	DiscGoalGeneratorDef(XmlElement const* xmlElement);
	virtual ~DiscGoalGeneratorDef() = default;

	float m_radius = 4.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct RectGoalGeneratorDef : public GoalGeneratorDef
{
	RectGoalGeneratorDef(XmlElement const* xmlElement);
	virtual ~RectGoalGeneratorDef() = default;

	IntVec2 m_dims = IntVec2(4, 4);
};



//----------------------------------------------------------------------------------------------------------------------
struct PerlinWormPathGeneratorDef : public MapGeneratorComponentDef
{
	PerlinWormPathGeneratorDef(XmlElement const* xmlElement);
	virtual ~PerlinWormPathGeneratorDef() = default;

	Vec2	m_startDir = Vec2(1.f, 0.f);
	Vec2	m_thicknessRange = Vec2(2.f, 5.f);
	float	m_thicknessVariance = 0.5f;
	float	m_splitChance = 0.0075f;
	float	m_splitAngleDeg = 35.f;
	int		m_maxSplits = 4;
};