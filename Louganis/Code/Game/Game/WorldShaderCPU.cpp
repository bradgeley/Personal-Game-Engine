// Bradley Christensen - 2022-2025
#include "WorldShaderCPU.h"



//----------------------------------------------------------------------------------------------------------------------
TerrainVertex::TerrainVertex(Vec3 const& pos, Rgba8 const& tint, Vec2 const& uvs, Vec2 const& lightmapUVs)
    : m_pos(pos)
    , m_tint(tint)
    , m_uvs(uvs)
    , m_lightmapUVs(lightmapUVs)
{
}