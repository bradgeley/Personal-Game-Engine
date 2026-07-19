// Bradley Christensen - 2022-2026
#include "MapGeneratorComponentDef.h"
#include "Engine/Core/ErrorUtils.h"



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
MapGeneratorComponentDef::MapGeneratorComponentDef(XmlElement const*)
{
	// Empty
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponentDef const* MapGeneratorComponentDef::MakeFromXmlElement(XmlElement const* xmlElement)
{	
	MapGeneratorComponentDef const* result = nullptr;

	Name xmlElementName = xmlElement->Name();
	
	// Biome Generator (unique per map, if present)
	if (xmlElementName == "BiomeGenerator")
	{
		result = new BiomeGeneratorComponentDef(xmlElement);
	}

	// Selectors
	if (xmlElementName == "NoiseRangeSelector")
	{
		result = new NoiseRangeSelectorDef(xmlElement);
	}

	if (xmlElementName == "NoisePeakSelector")
	{
		result = new NoisePeakSelectorDef(xmlElement);
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
		result = new RectGoalGeneratorDef(xmlElement);
	}

	if (xmlElementName == "DiscGoal")
	{
		result = new DiscGoalGeneratorDef(xmlElement);
	}

	// Path Gens
	if (xmlElementName == "PerlinWormPath")
	{
		result = new PerlinWormPathGeneratorDef(xmlElement);
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
TileSelectorDef::TileSelectorDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*xmlElement, "name", m_name);
}



//----------------------------------------------------------------------------------------------------------------------
NoiseSelectorDef::NoiseSelectorDef(XmlElement const* xmlElement) : TileSelectorDef(xmlElement)
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
NoiseRangeSelectorDef::NoiseRangeSelectorDef(XmlElement const* xmlElement) : NoiseSelectorDef(xmlElement)
{
	m_noiseRange = XmlUtils::ParseXmlAttribute(*xmlElement, "range", m_noiseRange);
}



//----------------------------------------------------------------------------------------------------------------------
NoisePeakSelectorDef::NoisePeakSelectorDef(XmlElement const* xmlElement) : NoiseSelectorDef(xmlElement)
{
	// Empty
}



//----------------------------------------------------------------------------------------------------------------------
BiomeGeneratorComponentDef::BiomeGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_biomeName = XmlUtils::ParseXmlAttribute(*xmlElement, "biome", m_biomeName);
}




//----------------------------------------------------------------------------------------------------------------------
DiscreteGeneratorComponentDef::DiscreteGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_tileSelectorName = XmlUtils::ParseXmlAttribute(*xmlElement, "selector", m_tileSelectorName);
}



//----------------------------------------------------------------------------------------------------------------------
TileGeneratorComponentDef::TileGeneratorComponentDef(XmlElement const* xmlElement) : DiscreteGeneratorComponentDef(xmlElement)
{
	m_tileName = XmlUtils::ParseXmlAttribute(*xmlElement, "tile", m_tileName);
}



//----------------------------------------------------------------------------------------------------------------------
EntityGeneratorComponentDef::EntityGeneratorComponentDef(XmlElement const* xmlElement) : DiscreteGeneratorComponentDef(xmlElement)
{
	m_entityName = XmlUtils::ParseXmlAttribute(*xmlElement, "entity", m_entityName);
}



//----------------------------------------------------------------------------------------------------------------------
GoalGeneratorDef::GoalGeneratorDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_mapAlignment = XmlUtils::ParseXmlAttribute(*xmlElement, "mapAlignment", m_mapAlignment);
}



//----------------------------------------------------------------------------------------------------------------------
DiscGoalGeneratorDef::DiscGoalGeneratorDef(XmlElement const* xmlElement) : GoalGeneratorDef(xmlElement)
{
	m_radius = XmlUtils::ParseXmlAttribute(*xmlElement, "radius", m_radius);
}



//----------------------------------------------------------------------------------------------------------------------
RectGoalGeneratorDef::RectGoalGeneratorDef(XmlElement const* xmlElement) : GoalGeneratorDef(xmlElement)
{
	m_dims = XmlUtils::ParseXmlAttribute(*xmlElement, "dims", m_dims);
}



//----------------------------------------------------------------------------------------------------------------------
PerlinWormPathGeneratorDef::PerlinWormPathGeneratorDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_startDir = XmlUtils::ParseXmlAttribute(*xmlElement, "startDir", m_startDir);
	m_thicknessRange = XmlUtils::ParseXmlAttribute(*xmlElement, "thicknessRange", m_thicknessRange);
	m_thicknessVariance = XmlUtils::ParseXmlAttribute(*xmlElement, "thicknessVariance", m_thicknessVariance);
	m_splitChance = XmlUtils::ParseXmlAttribute(*xmlElement, "splitChance", m_splitChance);
	m_splitAngleDeg = XmlUtils::ParseXmlAttribute(*xmlElement, "splitAngleDeg", m_splitAngleDeg);
	m_maxSplits = XmlUtils::ParseXmlAttribute(*xmlElement, "maxSplits", m_maxSplits);
}
