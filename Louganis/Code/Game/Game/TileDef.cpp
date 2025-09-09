// Bradley Christensen - 2022-2025
#include "TileDef.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_tileDefsFilePath = "Data/Definitions/TileDefs.xml";
std::vector<TileDef> TileDef::s_tileDefs;



//----------------------------------------------------------------------------------------------------------------------
TileDef::TileDef(XmlElement const* tileDefXmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "name", m_name);

	bool isVisible = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "visible", true);
	bool isSolid = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "solid", true);
	bool isOpaque = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "opaque", true);
	SetTags(isVisible, isSolid, isOpaque);

	m_tint = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "tint", m_tint);
	m_cost = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "cost", m_cost);
	m_spriteIndex = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "spriteIndex", m_spriteIndex);
}



//----------------------------------------------------------------------------------------------------------------------
void TileDef::SetTags(bool isVisible, bool isSolid, bool isOpaque)
{
	m_tags |= isVisible ? TileTag::Visible	: 0;
	m_tags |= isSolid	? TileTag::Solid	: 0;
	m_tags |= isOpaque	? TileTag::Opaque	: 0;
}



//----------------------------------------------------------------------------------------------------------------------
void TileDef::LoadFromXML()
{
	if (s_tileDefs.empty())
	{
		XmlDocument tileDefinitionsDoc;
		tileDefinitionsDoc.LoadFile(s_tileDefsFilePath);
		XmlElement const* tileDefElement = tileDefinitionsDoc.RootElement()->FirstChildElement();

		while (tileDefElement)
		{
			TileDef newTileDef(tileDefElement);
			s_tileDefs.push_back(newTileDef);
			tileDefElement = tileDefElement->NextSiblingElement();
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
TileDef const* TileDef::GetTileDef(uint8_t tileID)
{
	size_t index = static_cast<size_t>(tileID);
	if (index >= 0 && index <= s_tileDefs.size() - 1)
	{
		return &s_tileDefs[index];
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TileDef const* TileDef::GetTileDef(Name tileName)
{
	for (size_t i = 0; i < s_tileDefs.size(); i++)
	{
		TileDef const* def = &s_tileDefs[i];
		if (def->m_name == tileName)
		{
			return def;
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
int TileDef::GetTileDefID(Name name)
{
	for (int i = 0; i < (int) s_tileDefs.size(); ++i)
	{
		auto& def = s_tileDefs[i];
		if (def.m_name == name)
		{
			return i;
		}
	}
	return -1;
}