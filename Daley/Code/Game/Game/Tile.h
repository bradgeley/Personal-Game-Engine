// Bradley Christensen - 2022-2026
#pragma once
#include <cstdint>



typedef uint8_t TileTags;
typedef uint8_t TileID;



//----------------------------------------------------------------------------------------------------------------------
enum class TileTag : uint8_t
{
	None			= 0,
	Visible			= 1 << 0,
	Solid			= 1 << 1,
	Opaque			= 1 << 2,
	IsGoal			= 1 << 3,
	IsPath			= 1 << 4,
	Lighting_Dirty	= 1 << 5, // Means the lighting for this tile changed and its verts need to be recalculated
	Verts_Dirty		= 1 << 6, // Means the verts are dirty and need to be recalculated (e.g., because the tile ID changed, or lighting changed)
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
	inline bool IsGoal()			const { return m_tags & static_cast<uint8_t>(TileTag::IsGoal); }
	inline bool IsPath()			const { return m_tags & static_cast<uint8_t>(TileTag::IsPath); }
	inline bool IsLightingDirty()   const { return m_tags & static_cast<uint8_t>(TileTag::Lighting_Dirty); }
	inline bool IsVertsDirty()		const { return m_tags & static_cast<uint8_t>(TileTag::Verts_Dirty); }

	inline uint8_t GetOutdoorLighting() 	const { return m_lightingValues & 0x0F; }
	inline float   GetOutdoorLighting01()	const { return static_cast<float>(GetOutdoorLighting()) / 15.f; }
	inline uint8_t GetOutdoorLighting255()	const { return GetOutdoorLighting() * 17; }
	inline uint8_t GetIndoorLighting()		const { return (m_lightingValues & 0xF0) >> 4; }
	inline float   GetIndoorLighting01()	const { return static_cast<float>(GetIndoorLighting()) / 15.f; }
	inline uint8_t GetIndoorLighting255()	const { return GetIndoorLighting() * 17; }

	inline void SetIndoorLighting(uint8_t indoorLighting) { m_lightingValues = (m_lightingValues & 0x0F) | ((indoorLighting & 0x0F) << 4); } // Set the top 4 bits
	inline void SetOutdoorLighting(uint8_t outdoorLighting) { m_lightingValues = (m_lightingValues & 0xF0) | (outdoorLighting & 0x0F); }     // Set the bottom 4 bits

	void SetIsVisible(bool isVisible);
	void SetIsSolid(bool isSolid);
	void SetIsOpaque(bool isOpaque);
	void SetIsGoal(bool isGoal);
	void SetIsPath(bool isPath);
	void SetLightingDirty(bool dirty);
	void SetVertsDirty(bool dirty);

	bool operator==(Tile const& other) const;

public:

	TileID		m_id = 0;
	TileTags	m_tags = 0;
	uint8_t		m_lightingValues = 0;									// 4 bits indoor lighting (XXXX0000), 4 bits outdoor lighting (0000XXXX)
	uint8_t		m_staticLighting = 255;									// Brightness of this tile as determined by world generation (0-255)
};