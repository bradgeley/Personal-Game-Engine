// Bradley Christensen - 2022-2026
#include "MapGeneratorDef.h"
#include "BiomeDef.h"
#include "MapGeneratorComponentDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_mapGeneratorDefsFilePath = "Data/Definitions/MapGeneratorDefs.xml";
std::vector<MapGeneratorDef> MapGeneratorDef::s_mapGeneratorDefs;



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorDef::MapGeneratorDef(XmlElement const* mapGeneratorDefXmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*mapGeneratorDefXmlElement, "name", m_name);
	m_biome = XmlUtils::ParseXmlAttribute(*mapGeneratorDefXmlElement, "biome", m_biome);

	BiomeDef const* biomeDef = BiomeDef::GetBiomeDef(m_biome);
	ASSERT_OR_DIE(biomeDef != nullptr, StringUtils::StringF("MapGeneratorDef '%s' has invalid biome '%s'", m_name.ToCStr(), m_biome.ToCStr()));

	XmlElement const* mapGeneratorComponentDefElement = mapGeneratorDefXmlElement->FirstChildElement();
	while (mapGeneratorComponentDefElement)
	{
		MapGeneratorComponentDef const* mapGeneratorComponentDef = MapGeneratorComponentDef::MakeFromXmlElement(mapGeneratorComponentDefElement);
		if (mapGeneratorComponentDef)
		{
			m_mapGeneratorComponentDefs.emplace_back(const_cast<MapGeneratorComponentDef*>(mapGeneratorComponentDef));
		}
		else
		{
			ERROR_AND_DIE(StringUtils::StringF("Failed to create MapGeneratorComponentDef from XML element <%s> in MapGeneratorDef: %s", mapGeneratorComponentDefElement->Name(), m_name.ToCStr()));
		}
		mapGeneratorComponentDefElement = mapGeneratorComponentDefElement->NextSiblingElement();
	}
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorDef::~MapGeneratorDef()
{
	for (size_t i = 0; i < m_mapGeneratorComponentDefs.size(); ++i)
	{
		delete m_mapGeneratorComponentDefs[i];
	}
	m_mapGeneratorComponentDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorDef::MapGeneratorDef(MapGeneratorDef&& other) noexcept :
	m_name(other.m_name),
	m_mapGeneratorComponentDefs(std::move(other.m_mapGeneratorComponentDefs))
{
	other.m_mapGeneratorComponentDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorDef& MapGeneratorDef::operator=(MapGeneratorDef&& other) noexcept
{
	if (this != &other)
	{
		m_name = other.m_name;
		m_mapGeneratorComponentDefs = std::move(other.m_mapGeneratorComponentDefs);
		other.m_mapGeneratorComponentDefs.clear();
	}
	return *this;
}



//----------------------------------------------------------------------------------------------------------------------
void MapGeneratorDef::LoadFromXML()
{
	if (s_mapGeneratorDefs.empty())
	{
		XmlDocument mapGeneratorDefinitionsDoc;
		mapGeneratorDefinitionsDoc.LoadFile(s_mapGeneratorDefsFilePath);
		XmlElement const* mapGeneratorDefElement = mapGeneratorDefinitionsDoc.RootElement()->FirstChildElement();

		while (mapGeneratorDefElement)
		{
			s_mapGeneratorDefs.emplace_back(mapGeneratorDefElement);
			mapGeneratorDefElement = mapGeneratorDefElement->NextSiblingElement();
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void MapGeneratorDef::Shutdown()
{
	s_mapGeneratorDefs.clear();
}	



//----------------------------------------------------------------------------------------------------------------------
MapGeneratorDef const* MapGeneratorDef::GetMapGeneratorDef(Name name)
{
	for (size_t i = 0; i < s_mapGeneratorDefs.size(); i++)
	{
		MapGeneratorDef const* def = &s_mapGeneratorDefs[i];
		if (def->m_name == name)
		{
			return def;
		}
	}
	return nullptr;
}