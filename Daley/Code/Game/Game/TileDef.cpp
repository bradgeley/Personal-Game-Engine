// Bradley Christensen - 2022-2026
#include "TileDef.h"
#include "WorldSettings.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/ErrorUtils.h"



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
	m_indoorLight = XmlUtils::ParseXmlAttribute(*tileDefXmlElement, "indoorLight", m_indoorLight);
}



//----------------------------------------------------------------------------------------------------------------------
void TileDef::SetTags(bool isVisible, bool isSolid, bool isOpaque)
{
	m_tags |= isVisible ? (uint8_t) TileTag::Visible	: 0;
	m_tags |= isSolid	? (uint8_t) TileTag::Solid		: 0;
	m_tags |= isOpaque	? (uint8_t) TileTag::Opaque		: 0;
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
TileDef const* TileDef::GetTileDef(TileID tileID)
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
TileID TileDef::GetTileDefID(Name name)
{
	for (int i = 0; i < (int) s_tileDefs.size(); ++i)
	{
		auto& def = s_tileDefs[i];
		if (def.m_name == name)
		{
			return static_cast<TileID>(i);
		}
	}
	return static_cast<TileID>(-1);
}



//----------------------------------------------------------------------------------------------------------------------
Tile TileDef::GetDefaultTile(Name name)
{
	Tile result;
	TileID defID = GetTileDefID(name);
	if (defID != -1)
	{
		TileDef const* def = GetTileDef(defID);
		ASSERT_OR_DIE(def != nullptr, "TileDef::GetDefaultTile - failed to get tile def.");
		result.m_id = defID;
		result.m_tags = def->m_tags;
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
Tile TileDef::GetDefaultTile(TileID id)
{
	TileDef const* def = GetTileDef(id);
	ASSERT_OR_DIE(def != nullptr, "TileDef::GetDefaultTile - failed to get tile def.");
	return GetDefaultTile(def->m_name);
}
