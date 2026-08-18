// Bradley Christensen - 2022-2026
#include "BiomeDef.h"
#include "MapGeneratorComponentDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_biomeDefsFilePath = "Data/Definitions/BiomeDefs.xml";
std::vector<BiomeDef*> BiomeDef::s_biomeDefs;



//----------------------------------------------------------------------------------------------------------------------
BiomeDef::BiomeDef(XmlElement const* biomeDefXmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*biomeDefXmlElement, "name", m_name);
	m_baseTile = XmlUtils::ParseXmlAttribute(*biomeDefXmlElement, "baseTile", m_baseTile);
	m_goalTile = XmlUtils::ParseXmlAttribute(*biomeDefXmlElement, "goalTile", m_goalTile);
	m_pathTile = XmlUtils::ParseXmlAttribute(*biomeDefXmlElement, "pathTile", m_pathTile);

	XmlElement const* mapGeneratorComponentDefElement = biomeDefXmlElement->FirstChildElement();
	while (mapGeneratorComponentDefElement)
	{
		MapGeneratorComponentDef const* mapGeneratorComponentDef = MapGeneratorComponentDef::MakeFromXmlElement(mapGeneratorComponentDefElement);
		if (mapGeneratorComponentDef)
		{
			m_generatorComponentDefs.emplace_back(mapGeneratorComponentDef);
		}
		else
		{
			ERROR_AND_DIE(StringUtils::StringF("Failed to create MapGeneratorComponentDef from XML element <%s> in BiomeDef: %s", mapGeneratorComponentDefElement->Name(), m_name.ToCStr()));
		}
		mapGeneratorComponentDefElement = mapGeneratorComponentDefElement->NextSiblingElement();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void BiomeDef::Release()
{
	for (size_t i = 0; i < m_generatorComponentDefs.size(); ++i)
	{
		delete m_generatorComponentDefs[i];
	}
	m_generatorComponentDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void BiomeDef::LoadFromXML()
{
	if (s_biomeDefs.empty())
	{
		XmlDocument biomeDefinitionsDoc;
		biomeDefinitionsDoc.LoadFile(s_biomeDefsFilePath);
		XmlElement const* biomeDefElement = biomeDefinitionsDoc.RootElement()->FirstChildElement();

		while (biomeDefElement)
		{
			BiomeDef* biomeDef = new BiomeDef(biomeDefElement);
			s_biomeDefs.push_back(biomeDef);
			biomeDefElement = biomeDefElement->NextSiblingElement();
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void BiomeDef::Shutdown()
{
	for (size_t i = 0; i < s_biomeDefs.size(); ++i)
	{
		s_biomeDefs[i]->Release();
		delete s_biomeDefs[i];
	}
	s_biomeDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
BiomeDef const* BiomeDef::GetBiomeDef(Name name)
{
	for (size_t i = 0; i < s_biomeDefs.size(); i++)
	{
		BiomeDef const* def = s_biomeDefs[i];
		if (def->m_name == name)
		{
			return def;
		}
	}
	return nullptr;
}