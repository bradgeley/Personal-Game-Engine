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

	virtual class MapGeneratorComponent* MakeComponentInstance() const = 0;

	// Factory function to turn a generic componnet xml Element into an allocated def
	static MapGeneratorComponentDef const* MakeFromXmlElement(XmlElement const* xmlElement);
};



//----------------------------------------------------------------------------------------------------------------------
struct TileSelectorComponentDef : public MapGeneratorComponentDef
{
	TileSelectorComponentDef(XmlElement const* xmlElement);

	Name m_name = "Unnamed TileSelectorDef";
};



//----------------------------------------------------------------------------------------------------------------------
struct NoiseSelectorDef : public TileSelectorComponentDef
{
	NoiseSelectorDef(XmlElement const* xmlElement);
	~NoiseSelectorDef() = default;

	NoiseParams m_noiseParams;
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles in a range of noise values
//
struct NoiseRangeSelectorComponentDef : public NoiseSelectorDef
{
	NoiseRangeSelectorComponentDef(XmlElement const* xmlElement);
	~NoiseRangeSelectorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Vec2 m_noiseRange = Vec2(0.f, 1.f);
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles at local maxima of noise values
//
struct NoisePeakSelectorComponentDef : public NoiseSelectorDef
{
	NoisePeakSelectorComponentDef(XmlElement const* xmlElement);
	~NoisePeakSelectorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;
};



//----------------------------------------------------------------------------------------------------------------------
struct BiomeGeneratorComponentDef : public MapGeneratorComponentDef
{
	BiomeGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~BiomeGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_biomeName = "forest";
};



//----------------------------------------------------------------------------------------------------------------------
// Generators that use a discrete tile selector (one that returns a tile set to operate on)
//
struct DiscreteGeneratorComponentDef : public MapGeneratorComponentDef
{
	DiscreteGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~DiscreteGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_tileSelectorName;
};



//----------------------------------------------------------------------------------------------------------------------
struct TileGeneratorComponentDef : public DiscreteGeneratorComponentDef
{
	TileGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~TileGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_tileName = "grass";
};



//----------------------------------------------------------------------------------------------------------------------
struct EntityGeneratorComponentDef : public DiscreteGeneratorComponentDef
{
	EntityGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~EntityGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_entityName = "grass";
};


//----------------------------------------------------------------------------------------------------------------------
struct GoalGeneratorComponentDef : public MapGeneratorComponentDef
{
	GoalGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~GoalGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_tileName;
	Vec2 m_mapAlignment = Vec2(0.5f, 0.5f);
};



//----------------------------------------------------------------------------------------------------------------------
struct DiscGoalGeneratorComponentDef : public GoalGeneratorComponentDef
{
	DiscGoalGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~DiscGoalGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	float m_radius = 4.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct RectGoalGeneratorComponentDef : public GoalGeneratorComponentDef
{
	RectGoalGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~RectGoalGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	IntVec2 m_dims = IntVec2(4, 4);
};



//----------------------------------------------------------------------------------------------------------------------
struct PerlinWormPathGeneratorComponentDef : public MapGeneratorComponentDef
{
	PerlinWormPathGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~PerlinWormPathGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name	m_tileName;
	Vec2	m_startDir = Vec2(1.f, 0.f);
	Vec2	m_thicknessRange = Vec2(2.f, 5.f);
	float	m_thicknessVariance = 0.5f;
	float	m_splitChance = 0.0075f;
	float	m_splitAngleDeg = 35.f;
	int		m_maxSplits = 4;
};