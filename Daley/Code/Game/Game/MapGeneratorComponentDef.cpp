// Bradley Christensen - 2022-2026
#include "MapGeneratorComponentDef.h"
#include "MapGeneratorComponent.h"
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
		result = new NoiseRangeSelectorComponentDef(xmlElement);
	}

	if (xmlElementName == "NoisePeakSelector")
	{
		result = new NoisePeakSelectorComponentDef(xmlElement);
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
TileSelectorComponentDef::TileSelectorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*xmlElement, "name", m_name);
}



//----------------------------------------------------------------------------------------------------------------------
NoiseSelectorDef::NoiseSelectorDef(XmlElement const* xmlElement) : TileSelectorComponentDef(xmlElement)
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
NoiseRangeSelectorComponentDef::NoiseRangeSelectorComponentDef(XmlElement const* xmlElement) : NoiseSelectorDef(xmlElement)
{
	m_noiseRange = XmlUtils::ParseXmlAttribute(*xmlElement, "range", m_noiseRange);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* NoiseRangeSelectorComponentDef::MakeComponentInstance() const
{
	return new NoiseRangeSelectorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
NoisePeakSelectorComponentDef::NoisePeakSelectorComponentDef(XmlElement const* xmlElement) : NoiseSelectorDef(xmlElement)
{
	// Empty
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* NoisePeakSelectorComponentDef::MakeComponentInstance() const
{
	return new NoisePeakSelectorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
BiomeGeneratorComponentDef::BiomeGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_biomeName = XmlUtils::ParseXmlAttribute(*xmlElement, "biome", m_biomeName);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* BiomeGeneratorComponentDef::MakeComponentInstance() const
{
	return new BiomeGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
DiscreteGeneratorComponentDef::DiscreteGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_tileSelectorName = XmlUtils::ParseXmlAttribute(*xmlElement, "selector", m_tileSelectorName);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* DiscreteGeneratorComponentDef::MakeComponentInstance() const
{
	return new DiscreteGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
TileGeneratorComponentDef::TileGeneratorComponentDef(XmlElement const* xmlElement) : DiscreteGeneratorComponentDef(xmlElement)
{
	m_tileName = XmlUtils::ParseXmlAttribute(*xmlElement, "tile", m_tileName);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* TileGeneratorComponentDef::MakeComponentInstance() const
{
	return new TileGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
EntityGeneratorComponentDef::EntityGeneratorComponentDef(XmlElement const* xmlElement) : DiscreteGeneratorComponentDef(xmlElement)
{
	m_entityName = XmlUtils::ParseXmlAttribute(*xmlElement, "entity", m_entityName);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* EntityGeneratorComponentDef::MakeComponentInstance() const
{
	return new EntityGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
GoalGeneratorComponentDef::GoalGeneratorComponentDef(XmlElement const* xmlElement) : MapGeneratorComponentDef(xmlElement)
{
	m_mapAlignment = XmlUtils::ParseXmlAttribute(*xmlElement, "mapAlignment", m_mapAlignment);
	m_tileName = XmlUtils::ParseXmlAttribute(*xmlElement, "tile", m_tileName);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* GoalGeneratorComponentDef::MakeComponentInstance() const
{
	return new GoalGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
DiscGoalGeneratorComponentDef::DiscGoalGeneratorComponentDef(XmlElement const* xmlElement) : GoalGeneratorComponentDef(xmlElement)
{
	m_radius = XmlUtils::ParseXmlAttribute(*xmlElement, "radius", m_radius);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* DiscGoalGeneratorComponentDef::MakeComponentInstance() const
{
	return new DiscGoalGeneratorComponent(*this);
}



//----------------------------------------------------------------------------------------------------------------------
RectGoalGeneratorComponentDef::RectGoalGeneratorComponentDef(XmlElement const* xmlElement) : GoalGeneratorComponentDef(xmlElement)
{
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
	m_tileName = XmlUtils::ParseXmlAttribute(*xmlElement, "tile", m_tileName);
	m_startDir = XmlUtils::ParseXmlAttribute(*xmlElement, "startDir", m_startDir);
	m_thicknessRange = XmlUtils::ParseXmlAttribute(*xmlElement, "thicknessRange", m_thicknessRange);
	m_thicknessVariance = XmlUtils::ParseXmlAttribute(*xmlElement, "thicknessVariance", m_thicknessVariance);
	m_splitChance = XmlUtils::ParseXmlAttribute(*xmlElement, "splitChance", m_splitChance);
	m_splitAngleDeg = XmlUtils::ParseXmlAttribute(*xmlElement, "splitAngleDeg", m_splitAngleDeg);
	m_maxSplits = XmlUtils::ParseXmlAttribute(*xmlElement, "maxSplits", m_maxSplits);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* PerlinWormPathGeneratorComponentDef::MakeComponentInstance() const
{
	return new PerlinWormPathGeneratorComponent(*this);
}
