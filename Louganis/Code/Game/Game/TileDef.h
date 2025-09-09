// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Core/Name.h"
#include <vector>



typedef uint8_t TileTags;
typedef uint8_t TileID;



//----------------------------------------------------------------------------------------------------------------------
enum TileTag : uint8_t
{
	None		= 0,
	Visible		= 1 << 0,
	Solid		= 1 << 1,
	Opaque		= 1 << 2,
};



//----------------------------------------------------------------------------------------------------------------------
struct TileDef
{
public:

	explicit TileDef(XmlElement const* tileDefXmlElement);

	static void LoadFromXML();
	static TileDef const* GetTileDef(uint8_t id);
	static TileDef const* GetTileDef(Name name);
	static int GetTileDefID(Name name);

	inline bool IsVisible() const { return m_tags & TileTag::Visible; }
	inline bool IsSolid()   const { return m_tags & TileTag::Solid; }
	inline bool IsOpaque()  const { return m_tags & TileTag::Opaque; }

	void SetTags(bool isVisible, bool isSolid, bool isOpaque);

private:

	static std::vector<TileDef> s_tileDefs;

public:

	Name		m_name			= "Unnamed TileDef";
	uint8_t		m_spriteIndex	= 1;
	TileTags	m_tags			= 0;
	Rgba8		m_tint			= Rgba8::White;
	uint8_t		m_cost			= 1;
};