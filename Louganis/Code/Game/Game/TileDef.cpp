// Bradley Christensen - 2024
#include "TileDef.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_tileDefsFilePath = "Data/Definitions/TileDefs.xml";
std::vector<TileDef> TileDef::s_tileDefs;



//----------------------------------------------------------------------------------------------------------------------
TileDef::TileDef(XmlElement const* tileDefXmlElement)
{
	m_name = ParseXmlAttribute(*tileDefXmlElement, "name", m_name);

	bool isVisible = ParseXmlAttribute(*tileDefXmlElement, "visible", true);
	bool isSolid = ParseXmlAttribute(*tileDefXmlElement, "solid", true);
	bool isOpaque = ParseXmlAttribute(*tileDefXmlElement, "opaque", true);
	SetTags(isVisible, isSolid, isOpaque);

	m_tint = ParseXmlAttribute(*tileDefXmlElement, "tint", m_tint);
	m_cost = ParseXmlAttribute(*tileDefXmlElement, "cost", m_cost);
}



//----------------------------------------------------------------------------------------------------------------------
void TileDef::SetTags(bool isVisible, bool isSolid, bool isOpaque)
{
	if (isVisible) m_tags |= TileTag::Visible;
	if (isSolid)   m_tags |= TileTag::Solid;
	if (isOpaque)  m_tags |= TileTag::Opaque;
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
TileDef const* TileDef::GetTileDef(std::string const& tileName)
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
int TileDef::GetTileDefID(std::string const& name)
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