// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/TagQuery.h"
#include "Engine/Core/XmlUtils.h"



class MapGeneratorComponent;



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
	MapGeneratorComponentDef() = default;
	virtual ~MapGeneratorComponentDef() = default;

	virtual class MapGeneratorComponent* MakeComponentInstance() const = 0;

	// Factory function to turn a generic componnet xml Element into an allocated def
	static MapGeneratorComponentDef const* MakeFromXmlElement(XmlElement const* xmlElement);
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles in a range of noise values
//
struct NoiseRangeSelectorComponentDef : public MapGeneratorComponentDef
{
	NoiseRangeSelectorComponentDef(XmlElement const* xmlElement);
	~NoiseRangeSelectorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_name = "Unnamed SelectorDef";
	Vec2 m_noiseRange = Vec2(0.f, 1.f);
	NoiseParams m_noiseParams;
	TagQuery m_tagQuery;
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles at local maxima of noise values
//
struct NoisePeakSelectorComponentDef : public MapGeneratorComponentDef
{
	NoisePeakSelectorComponentDef(XmlElement const* xmlElement);
	~NoisePeakSelectorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_name = "Unnamed SelectorDef";
	NoiseParams m_noiseParams;
	TagQuery m_tagQuery;
};



//----------------------------------------------------------------------------------------------------------------------
struct TileGeneratorComponentDef : public MapGeneratorComponentDef
{
	TileGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~TileGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Name m_tileSelectorName;
	Name m_tileName = "grass";
};



//----------------------------------------------------------------------------------------------------------------------
struct EntityGeneratorComponentDef : public MapGeneratorComponentDef
{
	EntityGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~EntityGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;
	
	Name m_tileSelectorName;
	Name m_entityName = "grass";
};



//----------------------------------------------------------------------------------------------------------------------
struct DiscGoalGeneratorComponentDef : public MapGeneratorComponentDef
{
	DiscGoalGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~DiscGoalGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Vec2 m_alignment = Vec2(0.5f, 0.5f);
	float m_radius = 4.f;
};



//----------------------------------------------------------------------------------------------------------------------
struct RectGoalGeneratorComponentDef : public MapGeneratorComponentDef
{
	RectGoalGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~RectGoalGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Vec2 m_alignment = Vec2(0.5f, 0.5f);
	IntVec2 m_dims = IntVec2(4, 4);
};



//----------------------------------------------------------------------------------------------------------------------
struct PerlinWormPathGeneratorComponentDef : public MapGeneratorComponentDef
{
	PerlinWormPathGeneratorComponentDef(XmlElement const* xmlElement);
	virtual ~PerlinWormPathGeneratorComponentDef() = default;

	virtual MapGeneratorComponent* MakeComponentInstance() const override;

	Vec2	m_startPos = Vec2(0.f, 0.f); // 0-1 alignment of the start pos on the map
	Vec2	m_startDir = Vec2(1.f, 0.f);
	Vec2	m_thicknessRange = Vec2(2.f, 5.f);
	float	m_thicknessVariance = 0.5f;
	float	m_splitChance = 0.0075f;
	float	m_splitAngleDeg = 35.f;
	int		m_maxSplits = 4;
	int		m_seedOffset = 0;
};