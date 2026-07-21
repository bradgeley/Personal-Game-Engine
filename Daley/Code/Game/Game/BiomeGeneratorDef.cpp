// Bradley Christensen - 2022-2026
#include "BiomeGeneratorDef.h"
#include "MapGeneratorComponentDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_biomeGeneratorDefsFilePath = "Data/Definitions/BiomeGeneratorDefs.xml";
std::vector<BiomeGeneratorDef> BiomeGeneratorDef::s_biomeGeneratorDefs;



//----------------------------------------------------------------------------------------------------------------------
BiomeGeneratorDef::BiomeGeneratorDef(XmlElement const* biomeGeneratorDefXmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*biomeGeneratorDefXmlElement, "name", m_name);
	m_baseTile = XmlUtils::ParseXmlAttribute(*biomeGeneratorDefXmlElement, "baseTile", m_baseTile);

	XmlElement const* mapGeneratorComponentDefElement = biomeGeneratorDefXmlElement->FirstChildElement();
	while (mapGeneratorComponentDefElement)
	{
		MapGeneratorComponentDef const* mapGeneratorComponentDef = MapGeneratorComponentDef::MakeFromXmlElement(mapGeneratorComponentDefElement);
		if (mapGeneratorComponentDef)
		{
			m_generatorComponentDefs.push_back(mapGeneratorComponentDef);
		}
		else
		{
			ERROR_AND_DIE(StringUtils::StringF("Failed to create MapGeneratorComponentDef from XML element <%s> in BiomeGeneratorDef: %s", mapGeneratorComponentDefElement->Name(), m_name.ToCStr()));
		}
		mapGeneratorComponentDefElement = mapGeneratorComponentDefElement->NextSiblingElement();
	}
}



//----------------------------------------------------------------------------------------------------------------------
BiomeGeneratorDef::~BiomeGeneratorDef()
{
	for (size_t i = 0; i < m_generatorComponentDefs.size(); ++i)
	{
		delete m_generatorComponentDefs[i];
	}
	m_generatorComponentDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void BiomeGeneratorDef::LoadFromXML()
{
	if (s_biomeGeneratorDefs.empty())
	{
		XmlDocument biomeGeneratorDefinitionsDoc;
		biomeGeneratorDefinitionsDoc.LoadFile(s_biomeGeneratorDefsFilePath);
		XmlElement const* biomeGeneratorDefElement = biomeGeneratorDefinitionsDoc.RootElement()->FirstChildElement();

		while (biomeGeneratorDefElement)
		{
			s_biomeGeneratorDefs.emplace_back(biomeGeneratorDefElement);
			biomeGeneratorDefElement = biomeGeneratorDefElement->NextSiblingElement();
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void BiomeGeneratorDef::Shutdown()
{
	s_biomeGeneratorDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
BiomeGeneratorDef const* BiomeGeneratorDef::GetBiomeGeneratorDef(Name name)
{
	for (size_t i = 0; i < s_biomeGeneratorDefs.size(); i++)
	{
		BiomeGeneratorDef const* def = &s_biomeGeneratorDefs[i];
		if (def->m_name == name)
		{
			return def;
		}
	}
	return nullptr;
}