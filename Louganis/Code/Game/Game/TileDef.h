// Bradley Christensen - 2024
#pragma once
#include "Engine/Math/IntVec2.h"
#include "Engine/Math/AABB2.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Renderer/Rgba8.h"
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
	static TileDef const* GetTileDef(std::string const& name);
	static int GetTileDefID(std::string const& name);

	inline bool IsVisible() const { return m_tags & TileTag::Visible; }
	inline bool IsSolid()   const { return m_tags & TileTag::Solid; }
	inline bool IsOpaque()  const { return m_tags & TileTag::Opaque; }

	void SetTags(bool isVisible, bool isSolid, bool isOpaque);

private:

	static std::vector<TileDef> s_tileDefs;

public:

	std::string m_name = "Unnamed TileDef";
	AABB2		m_UVs;
	TileTags	m_tags;
	Rgba8		m_tint = Rgba8::White;
};