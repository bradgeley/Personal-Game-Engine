// Bradley Christensen - 2022-2023
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU(Vec2 const& position, Rgba8 const& tint, Vec2 const& uvs)
    : pos(Vec3(position)), tint(tint), uvs(uvs)
{}



//----------------------------------------------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU(Vec3 const& position, Rgba8 const& tint, Vec2 const& uvs)
    : pos(position), tint(tint), uvs(uvs)
{}
