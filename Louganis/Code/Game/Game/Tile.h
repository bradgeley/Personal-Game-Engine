// Bradley Christensen - 2022-2025
#pragma once
#include <cstdint>



typedef uint8_t TileTags;
typedef uint8_t TileID;



//----------------------------------------------------------------------------------------------------------------------
enum class TileTag : uint8_t
{
	None			= 0,
	VBO_Dirty		= 1 << 1, // Means the lighting for this tile changed and its verts need to be recalculated
	Lighting_Dirty	= 1 << 2, // Means the lighting for this tile changed and its verts need to be recalculated
	Visible			= 1 << 3,
	Solid			= 1 << 4,
	Opaque			= 1 << 5,
};



//----------------------------------------------------------------------------------------------------------------------
struct Tile
{
public:

	Tile() = default;
	Tile(uint8_t tileID, uint8_t lighting = 0, uint8_t flags = 0);

	inline bool IsVisible()			const { return m_tags & static_cast<uint8_t>(TileTag::Visible); }
	inline bool IsSolid()			const { return m_tags & static_cast<uint8_t>(TileTag::Solid); }
	inline bool IsOpaque()			const { return m_tags & static_cast<uint8_t>(TileTag::Opaque); }
	inline bool IsVBODirty()		const { return m_tags & static_cast<uint8_t>(TileTag::VBO_Dirty); }
	inline bool IsLightingDirty()   const { return m_tags & static_cast<uint8_t>(TileTag::Lighting_Dirty); }

protected:

	friend class Chunk;

	void SetDirty(bool dirty);

public:

	TileID		m_id				= 0;
	TileTags	m_tags				= (TileTags) TileTag::VBO_Dirty & (TileTags) TileTag::Lighting_Dirty;	// Start tiles off as dirty
	uint8_t		m_lightingValue		= 0;							// 4 bits indoor lighting (XXXX0000), 4 bits outdoor lighting (0000XXXX)
	uint8_t		m_staticLighting	= 0;							// Brightness of this tile as determined by world generation (0-255)
};