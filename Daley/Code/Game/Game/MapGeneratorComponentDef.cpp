// Bradley Christensen - 2022-2026
#include "MapGeneratorComponentDef.h"
#include "MapGeneratorComponent.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



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
	int seedOffsetAsInt = XmlUtils::ParseXmlAttribute(*xmlElement, "seedOffset", 0);
	ASSERT_OR_DIE(seedOffsetAsInt >= 0, "seedOffset must be non-negative");
	m_seedOffset = static_cast<uint32_t>(seedOffsetAsInt);
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorComponent* PerlinWormPathGeneratorComponentDef::MakeComponentInstance() const
{
	return new PerlinWormPathGeneratorComponent(*this);
}