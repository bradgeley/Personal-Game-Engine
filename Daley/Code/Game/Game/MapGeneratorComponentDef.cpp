// Bradley Christensen - 2022-2026
#include "MapGeneratorComponentDef.h"
#include "MapGeneratorComponent.h"
#include "Tile.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/TagQuery.h"



//----------------------------------------------------------------------------------------------------------------------
TagQuery MakeTileTagQueryFromXmlElement(XmlElement const& xmlElement)
{
	TagQuery result;
	Name queryOpName = XmlUtils::ParseXmlAttribute(xmlElement, "queryOp", Name("all"));
	if (queryOpName == "all")
	{
		result.m_queryOp = TagQueryOp::All;
	}
	else if (queryOpName == "any")
	{
		result.m_queryOp = TagQueryOp::Any;
	}
	else
	{
		ERROR_AND_DIE("Invalid queryOp attribute on TagQuery element");
	}

	const char* dataNames[4] = { "hasAll", "hasAny", "doesNotHaveAll", "doesNotHaveAny" };
	uint8_t* tagQueryData[4] = { &result.m_hasAllTags, &result.m_hasAnyTags, &result.m_doesNotHaveAllTags, &result.m_doesNotHaveAnyTags };

	for (int i = 0; i < 4; ++i)
	{
		const char* dataName = dataNames[i];
		uint8_t* tagQueryDataPtr = tagQueryData[i];
		std::string tagsStr = XmlUtils::ParseXmlAttribute(xmlElement, dataName, "");
		if (tagsStr.empty())
		{
			continue;
		}

		Strings tags = StringUtils::SplitStringOnDelimiter(tagsStr, ',');
		ASSERT_OR_DIE(tags.size() > 0, "No tags in TagQuery element.");

		for (std::string const& tagName : tags)
		{
			if (Name(tagName) == "path")
			{
				*tagQueryDataPtr |= (uint8_t) TileTag::IsPath;
			}
			else if (Name(tagName) == "goal")
			{
				*tagQueryDataPtr |= (uint8_t) TileTag::IsGoal;
			}
			else if (Name(tagName) == "solid")
			{
				*tagQueryDataPtr |= (uint8_t) TileTag::Solid;
			}
			else if (Name(tagName) == "opaque")
			{
				*tagQueryDataPtr |= (uint8_t) TileTag::Opaque;
			}
			else if(Name(tagName) == "visible")
			{
				*tagQueryDataPtr |= (uint8_t) TileTag::Visible;
			}
			else if (Name(tagName) == "blockstowers")
			{
				*tagQueryDataPtr |= (uint8_t) TileTag::BlocksTowers;
			}
			else
			{
				ERROR_AND_DIE("Invalid tag name in attribute on TagQuery element");
			}
		}
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
NoiseParams::NoiseParams(XmlElement const* xmlElement)
{
	m_outputRange = XmlUtils::ParseXmlAttribute(*xmlElement, "outputRange", m_outputRange);
	m_scale = XmlUtils::ParseXmlAttribute(*xmlElement, "scale", m_scale);
	m_numOctaves = XmlUtils::ParseXmlAttribute(*xmlElement, "numOctaves", m_numOctaves);
	m_octavePersistence = XmlUtils::ParseXmlAttribute(*xmlElement, "octavePersistence", m_octavePersistence);
	m_octaveScale = XmlUtils::ParseXmlAttribute(*xmlElement, "octaveScale", m_octaveScale);
	m_renormalize = XmlUtils::ParseXmlAttribute(*xmlElement, "renormalize", m_renormalize);
	m_seedOffset = XmlUtils::ParseXmlAttribute(*xmlElement, "seedOffset", m_seedOffset);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponentDef::MapGeneratorComponentDef(XmlElement const* xmlElement)
{
	m_runOrder = XmlUtils::ParseXmlAttribute(*xmlElement, "runOrder", m_runOrder);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponentDef const* MapGeneratorComponentDef::MakeFromXmlElement(XmlElement const* xmlElement)
{	
	MapGeneratorComponentDef const* result = nullptr;

	Name xmlElementName = xmlElement->Name();

	// Selectors
	if (xmlElementName == "NoiseRangeSelector")
	{
		result = new NoiseRangeSelectorComponentDef(xmlElement);
	}

	if (xmlElementName == "NoisePeakSelector")
	{
		result = new NoisePeakSelectorComponentDef(xmlElement);
	}

	if (xmlElementName == "DistanceFieldSelector")
	{
		result = new DistanceFieldSelectorComponentDef(xmlElement);
	}

	// Discrete Generators
	if (xmlElementName == "TileGenerator")
	{
		result = new TileGeneratorComponentDef(xmlElement);
	}

	if (xmlElementName == "EntityGenerator")
	{
		result = new EntityGeneratorComponentDef(xmlElement);
	}

	// Goal Gens
	if (xmlElementName == "RectGoal")
	{
		result = new RectGoalGeneratorComponentDef(xmlElement);
	}

	if (xmlElementName == "DiscGoal")
	{
		result = new DiscGoalGeneratorComponentDef(xmlElement);
	}

	// Path Gens
	if (xmlElementName == "PerlinWormPath")
	{
		result = new PerlinWormPathGeneratorComponentDef(xmlElement);
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
NoiseRangeSelectorComponentDef::NoiseRangeSelectorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*xmlElement, "name", m_name);
	m_noiseRange = XmlUtils::ParseXmlAttribute(*xmlElement, "range", m_noiseRange);

	XmlElement const* noiseParamsElem = xmlElement->FirstChildElement("NoiseParams");
	if (noiseParamsElem)
	{
		m_noiseParams = NoiseParams(noiseParamsElem);
	}
	else
	{
		ERROR_AND_DIE("NoiseSelectorDef requires a NoiseParams child element");
	}

	XmlElement const* tagQueryElem = xmlElement->FirstChildElement("TagQuery");
	if (tagQueryElem)
	{
		m_tagQuery = MakeTileTagQueryFromXmlElement(*tagQueryElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* NoiseRangeSelectorComponentDef::MakeComponentInstance() const
{
	return new NoiseRangeSelectorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
NoisePeakSelectorComponentDef::NoisePeakSelectorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*xmlElement, "name", m_name);

	XmlElement const* noiseParamsElem = xmlElement->FirstChildElement("NoiseParams");
	if (noiseParamsElem)
	{
		m_noiseParams = NoiseParams(noiseParamsElem);
	}
	else
	{
		ERROR_AND_DIE("NoiseSelectorDef requires a NoiseParams child element");
	}

	XmlElement const* tagQueryElem = xmlElement->FirstChildElement("TagQuery");
	if (tagQueryElem)
	{
		m_tagQuery = MakeTileTagQueryFromXmlElement(*tagQueryElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* NoisePeakSelectorComponentDef::MakeComponentInstance() const
{
	return new NoisePeakSelectorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
DistanceFieldSelectorComponentDef::DistanceFieldSelectorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*xmlElement, "name", m_name);

	m_distanceRange = XmlUtils::ParseXmlAttribute(*xmlElement, "range", m_distanceRange);

	XmlElement const* tagQueryElem = xmlElement->FirstChildElement("TagQuery");
	ASSERT_OR_DIE(tagQueryElem != nullptr, "DistanceFieldSelectorDef requires a TagQuery child element");

	if (tagQueryElem)
	{
		m_tagQuery = MakeTileTagQueryFromXmlElement(*tagQueryElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* DistanceFieldSelectorComponentDef::MakeComponentInstance() const
{
	return new DistanceFieldSelectorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
TileGeneratorComponentDef::TileGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_tileSelectorName = XmlUtils::ParseXmlAttribute(*xmlElement, "selector", m_tileSelectorName);
	m_tileName = XmlUtils::ParseXmlAttribute(*xmlElement, "tile", m_tileName);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* TileGeneratorComponentDef::MakeComponentInstance() const
{
	return new TileGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
EntityGeneratorComponentDef::EntityGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_tileSelectorName = XmlUtils::ParseXmlAttribute(*xmlElement, "selector", m_tileSelectorName);
	m_entityName = XmlUtils::ParseXmlAttribute(*xmlElement, "entity", m_entityName);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* EntityGeneratorComponentDef::MakeComponentInstance() const
{
	return new EntityGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
DiscGoalGeneratorComponentDef::DiscGoalGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_alignment = XmlUtils::ParseXmlAttribute(*xmlElement, "alignment", m_alignment);
	m_radius = XmlUtils::ParseXmlAttribute(*xmlElement, "radius", m_radius);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* DiscGoalGeneratorComponentDef::MakeComponentInstance() const
{
	return new DiscGoalGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
RectGoalGeneratorComponentDef::RectGoalGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_alignment = XmlUtils::ParseXmlAttribute(*xmlElement, "alignment", m_alignment);
	m_dims = XmlUtils::ParseXmlAttribute(*xmlElement, "dims", m_dims);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* RectGoalGeneratorComponentDef::MakeComponentInstance() const
{
	return new RectGoalGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
PerlinWormPathGeneratorComponentDef::PerlinWormPathGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_startPos = XmlUtils::ParseXmlAttribute(*xmlElement, "startPos", m_startPos);
	m_startDir = XmlUtils::ParseXmlAttribute(*xmlElement, "startDir", m_startDir);
	m_thicknessRange = XmlUtils::ParseXmlAttribute(*xmlElement, "thicknessRange", m_thicknessRange);
	m_thicknessNoiseScale = XmlUtils::ParseXmlAttribute(*xmlElement, "thicknessNoiseScale", m_thicknessNoiseScale);
	m_splitChance = XmlUtils::ParseXmlAttribute(*xmlElement, "splitChance", m_splitChance);
	m_splitAngleDeg = XmlUtils::ParseXmlAttribute(*xmlElement, "splitAngleDeg", m_splitAngleDeg);
	m_maxSplits = XmlUtils::ParseXmlAttribute(*xmlElement, "maxSplits", m_maxSplits);
	m_seedOffset = XmlUtils::ParseXmlAttribute(*xmlElement, "seedOffset", m_seedOffset);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* PerlinWormPathGeneratorComponentDef::MakeComponentInstance() const
{
	return new PerlinWormPathGeneratorComponent(*this);
}
