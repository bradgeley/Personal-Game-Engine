// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



typedef uint8_t TileTags;
typedef uint8_t TileID;



//----------------------------------------------------------------------------------------------------------------------
enum class TileTag : uint8_t
{
	None		= 0,
	Dirty		= 1 << 1, // Means the lighting for this tile changed and its verts need to be recalculated
	Visible		= 1 << 2,
	Solid		= 1 << 3,
	Opaque		= 1 << 4,
};



//----------------------------------------------------------------------------------------------------------------------
struct Tile
{
public:

	Tile() = default;
	Tile(uint8_t tileID, uint8_t lighting = 0, uint8_t flags = 0);

	inline bool IsVisible() const { return m_tags & static_cast<uint8_t>(TileTag::Visible); }
	inline bool IsSolid()   const { return m_tags & static_cast<uint8_t>(TileTag::Solid); }
	inline bool IsOpaque()  const { return m_tags & static_cast<uint8_t>(TileTag::Opaque); }
	inline bool IsDirty()   const { return m_tags & static_cast<uint8_t>(TileTag::Dirty); }

	void SetDirty(bool dirty);

public:

	TileID		m_id				= 0;
	TileTags	m_tags				= (TileTags) TileTag::Dirty;	// Start tiles off as dirty
	uint8_t		m_lightingValue		= 0xF0;							// 4 bits static lighting (XXXX0000) from world height, 4 bits dynamic lighting (0000XXXX)
};