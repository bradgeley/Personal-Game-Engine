// Bradley Christensen - 2022-2026
#include "Tile.h"



//----------------------------------------------------------------------------------------------------------------------
Tile::Tile(uint8_t tileID, uint8_t lighting, uint8_t flags) :
    m_id(tileID),
    m_tags(flags),
    m_lightingValues(lighting)
{

}



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetIsVisible(bool isVisible)
{
    if (isVisible)
    {
        m_tags |= static_cast<uint8_t>(TileTag::Visible);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::Visible);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetIsSolid(bool isSolid)
{
    if (isSolid)
    {
        m_tags |= static_cast<uint8_t>(TileTag::Solid);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::Solid);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetIsOpaque(bool isOpaque)
{
    if (isOpaque)
    {
        m_tags |= static_cast<uint8_t>(TileTag::Opaque);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::Opaque);
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



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetIsPath(bool isPath)
{
    if (isPath)
    {
        m_tags |= static_cast<uint8_t>(TileTag::IsPath);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::IsPath);
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



//----------------------------------------------------------------------------------------------------------------------
void Tile::SetVertsDirty(bool dirty)
{
    if (dirty)
    {
        m_tags |= static_cast<uint8_t>(TileTag::Verts_Dirty);
    }
    else
    {
        m_tags &= ~static_cast<uint8_t>(TileTag::Verts_Dirty);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool Tile::operator==(Tile const& other) const
{
    return m_id == other.m_id
        && m_tags == other.m_tags
        && m_lightingValues == other.m_lightingValues
		&& m_staticLighting == other.m_staticLighting;
}
