// Bradley Christensen - 2022-2026
#include "TileSelectorComponentDef.h"
#include "TileSelectorComponent.h"
#include "Tile.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



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
			else if (Name(tagName) == "occupied")
			{
				*tagQueryDataPtr |= (uint8_t) TileTag::Occupied;
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
TileSelectorComponentDef::TileSelectorComponentDef(XmlElement const* xmlElement)
{
	XmlElement const& element = *xmlElement;
	m_name = XmlUtils::ParseXmlAttribute(element, "name", m_name);
	m_chanceToSelect = XmlUtils::ParseXmlAttribute(element, "chance", m_chanceToSelect);

	XmlElement const* tagQueryElem = xmlElement->FirstChildElement("TagQuery");
	if (tagQueryElem)
	{
		m_tagQuery = MakeTileTagQueryFromXmlElement(*tagQueryElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponentDef const* TileSelectorComponentDef::MakeFromXmlElement(XmlElement const* xmlElement)
{
	TileSelectorComponentDef const* result = nullptr;

	Name xmlElementName = xmlElement->Name();

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

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
NoiseRangeSelectorComponentDef::NoiseRangeSelectorComponentDef(XmlElement const* xmlElement) : TileSelectorComponentDef(xmlElement)
{
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
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent* NoiseRangeSelectorComponentDef::MakeComponentInstance() const
{
	return new NoiseRangeSelectorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
NoisePeakSelectorComponentDef::NoisePeakSelectorComponentDef(XmlElement const* xmlElement) : TileSelectorComponentDef(xmlElement)
{
	XmlElement const* noiseParamsElem = xmlElement->FirstChildElement("NoiseParams");
	if (noiseParamsElem)
	{
		m_noiseParams = NoiseParams(noiseParamsElem);
	}
	else
	{
		ERROR_AND_DIE("NoiseSelectorDef requires a NoiseParams child element");
	}
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent* NoisePeakSelectorComponentDef::MakeComponentInstance() const
{
	return new NoisePeakSelectorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
DistanceFieldSelectorComponentDef::DistanceFieldSelectorComponentDef(XmlElement const* xmlElement) : TileSelectorComponentDef(xmlElement)
{
	m_distanceRange = XmlUtils::ParseXmlAttribute(*xmlElement, "range", m_distanceRange);
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorComponent* DistanceFieldSelectorComponentDef::MakeComponentInstance() const
{
	return new DistanceFieldSelectorComponent(*this);
}