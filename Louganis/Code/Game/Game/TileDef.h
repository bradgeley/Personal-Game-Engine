// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Core/Name.h"
#include "Tile.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct TileDef
{
public:

	explicit TileDef(XmlElement const* tileDefXmlElement);

	static void LoadFromXML();
	static TileDef const* GetTileDef(TileID id);
	static TileDef const* GetTileDef(Name name);
	static int GetTileDefID(Name name);
	static Tile GetDefaultTile(Name name);

	inline bool IsVisible() const { return m_tags & static_cast<uint8_t>(TileTag::Visible); }
	inline bool IsSolid()   const { return m_tags & static_cast<uint8_t>(TileTag::Solid); }
	inline bool IsOpaque()  const { return m_tags & static_cast<uint8_t>(TileTag::Opaque); }

	void SetTags(bool isVisible, bool isSolid, bool isOpaque);

private:

	static std::vector<TileDef> s_tileDefs;

public:

	Name		m_name			= "Unnamed TileDef";
	uint8_t		m_spriteIndex	= 0;
	TileTags	m_tags = (TileTags) TileTag::VBO_Dirty & (TileTags) TileTag::Lighting_Dirty;	// Start tiles off as dirty
	Rgba8		m_tint			= Rgba8::White;
	uint8_t		m_cost			= 1;
};