// Bradley Christensen - 2022-2025
#include "Tile.h"



//----------------------------------------------------------------------------------------------------------------------
Tile::Tile(uint8_t tileID, uint8_t lighting, uint8_t flags) :
    m_id(tileID),
    m_tags(flags),
    m_lightingValues(lighting)
{

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



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetIsGoal(bool isGoal)
{
    if (isGoal)
    {
        m_tags |= static_cast<uint8_t>(TileTag::IsGoal);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::IsGoal);
	}
}
