// Bradley Christensen - 2022-2026
#pragma once
#include "NoiseParams.h"
#include "Engine/Core/Name.h"
#include "Engine/Core/TagQuery.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/Vec2.h"



class TileSelectorComponent;



//----------------------------------------------------------------------------------------------------------------------
struct TileSelectorComponentDef
{
	TileSelectorComponentDef(XmlElement const* xmlElement);
	virtual ~TileSelectorComponentDef() = default;

	virtual TileSelectorComponent* MakeComponentInstance() const = 0;

	// Factory function to turn a generic selector xml Element into an allocated def
	static TileSelectorComponentDef const* MakeFromXmlElement(XmlElement const* xmlElement);

	Name m_name = "Unnamed SelectorDef";
	float m_chanceToSelect = 1.f;
	TagQuery m_tagQuery;
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles in a range of noise values
//
struct NoiseRangeSelectorComponentDef : public TileSelectorComponentDef
{
	NoiseRangeSelectorComponentDef(XmlElement const* xmlElement);
	~NoiseRangeSelectorComponentDef() = default;

	virtual TileSelectorComponent* MakeComponentInstance() const override;

	Vec2 m_noiseRange = Vec2(0.f, 1.f);
	NoiseParams m_noiseParams;
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles at local maxima of noise values
//
struct NoisePeakSelectorComponentDef : public TileSelectorComponentDef
{
	NoisePeakSelectorComponentDef(XmlElement const* xmlElement);
	~NoisePeakSelectorComponentDef() = default;

	virtual TileSelectorComponent* MakeComponentInstance() const override;

	NoiseParams m_noiseParams;
};



//----------------------------------------------------------------------------------------------------------------------
// Places tiles in a range of distance field values based on a tile tag query
//
struct DistanceFieldSelectorComponentDef : public TileSelectorComponentDef
{
	DistanceFieldSelectorComponentDef(XmlElement const* xmlElement);
	~DistanceFieldSelectorComponentDef() = default;

	virtual TileSelectorComponent* MakeComponentInstance() const override;

	IntVec2 m_distanceRange = IntVec2(0, 999);
};