// Bradley Christensen - 2022-2026
#include "MapGeneratorDef.h"
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

	XmlElement const* mapGeneratorComponentDefElement = mapGeneratorDefXmlElement->FirstChildElement();
	while (mapGeneratorComponentDefElement)
	{
		MapGeneratorComponentDef const* mapGeneratorComponentDef = MapGeneratorComponentDef::MakeFromXmlElement(mapGeneratorComponentDefElement);
		if (mapGeneratorComponentDef)
		{
			m_mapGeneratorComponentDefs.push_back(const_cast<MapGeneratorComponentDef*>(mapGeneratorComponentDef));
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
void MapGeneratorDef::LoadFromXML()
{
	if (s_mapGeneratorDefs.empty())
	{
		XmlDocument mapGeneratorDefinitionsDoc;
		mapGeneratorDefinitionsDoc.LoadFile(s_mapGeneratorDefsFilePath);
		XmlElement const* mapGeneratorDefElement = mapGeneratorDefinitionsDoc.RootElement()->FirstChildElement();

		while (mapGeneratorDefElement)
		{
			MapGeneratorDef newMapGeneratorDef(mapGeneratorDefElement);
			s_mapGeneratorDefs.push_back(newMapGeneratorDef);
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