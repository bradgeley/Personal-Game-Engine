// Bradley Christensen - 2022-2025
#include "Tile.h"



//----------------------------------------------------------------------------------------------------------------------
Tile::Tile(uint8_t tileID, uint8_t lighting, uint8_t flags) :
    m_id(tileID),
	m_tags(flags),
    m_lightingValue(lighting)
{

}



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetVBODirty(bool dirty)
{
    if (dirty)
    {
        m_tags |= static_cast<uint8_t>(TileTag::VBO_Dirty);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::VBO_Dirty);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetLightingDirty(bool dirty)
{
    if (dirty)
    {
        m_tags |= static_cast<uint8_t>(TileTag::Lighting_Dirty);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::Lighting_Dirty);
    }
}
