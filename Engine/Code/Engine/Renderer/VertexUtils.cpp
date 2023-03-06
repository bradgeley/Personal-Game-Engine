// Bradley Christensen - 2022-2023
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Math/AABB2.h"



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForLine2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& tint)
{
    float halfThickness = thickness * 0.5f;
    Vec2 lineDir = (end - start).GetNormalized();
    Vec2 lineRotated90 = lineDir.GetRotated90();

    // Could be rotated but using an upright line as example for naming
    Vec2 botLeftCorner  = start - lineDir * halfThickness + lineRotated90 * halfThickness;
    Vec2 botRightCorner = start - lineDir * halfThickness - lineRotated90 * halfThickness;
    Vec2 topRightCorner = end + lineDir * halfThickness - lineRotated90 * halfThickness;
    Vec2 topLeftCorner  = end + lineDir * halfThickness + lineRotated90 * halfThickness;
    
    // Push some verts
    out_verts.reserve(out_verts.size() + 6);
    out_verts.emplace_back(Vec3(botLeftCorner), tint);
    out_verts.emplace_back(Vec3(botRightCorner), tint);
    out_verts.emplace_back(Vec3(topRightCorner), tint);
    
    out_verts.emplace_back(Vec3(topRightCorner), tint);
    out_verts.emplace_back(Vec3(topLeftCorner), tint);
    out_verts.emplace_back(Vec3(botLeftCorner), tint);
}



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB2(std::vector<Vertex_PCU>& out_verts, AABB2 const& square, Rgba8 const& tint, AABB2 const& UVs)
{
    AddVertsForRect2D(out_verts, square.mins, square.maxs, tint, UVs);
}



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForRect2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& mins, Vec2 const& maxs, Rgba8 const& tint, AABB2 const& UVs)
{
    // Get corners
    Vec3 bottomRightPoint = Vec3(maxs.x, mins.y, 1.f);
    Vec3 topRightPoint = Vec3(maxs.x, maxs.y, 1.f);
    Vec3 bottomLeftPoint = Vec3(mins.x, mins.y, 1.f);
    Vec3 topLeftPoint = Vec3(mins.x, maxs.y, 1.f);

    // Get UVs
    Vec2 const& topRightUVs = UVs.maxs;
    Vec2 const& bottomLeftUVs = UVs.mins;
    Vec2 bottomRightUVs = Vec2(UVs.maxs.x, UVs.mins.y);
    Vec2 topLeftUVs = Vec2(UVs.mins.x, UVs.maxs.y);

    // Push some verts
    out_verts.reserve(out_verts.size() + 6);
    out_verts.emplace_back(bottomLeftPoint, tint, bottomLeftUVs);
    out_verts.emplace_back(bottomRightPoint, tint, bottomRightUVs);
    out_verts.emplace_back(topRightPoint, tint, topRightUVs);
    
    out_verts.emplace_back(topRightPoint, tint, topRightUVs);
    out_verts.emplace_back(topLeftPoint, tint, topLeftUVs);
    out_verts.emplace_back(bottomLeftPoint, tint, bottomLeftUVs);
}



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForWireBox2D(std::vector<Vertex_PCU>& out_verts, AABB2 const& box, float lineThickness, Rgba8 const& tint)
{
    AddVertsForWireBox2D(out_verts, box.mins, box.maxs, lineThickness, tint);
}



//----------------------------------------------------------------------------------------------------------------------
void AddVertsForWireBox2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& mins, Vec2 const& maxs, float lineThickness, Rgba8 const& tint)
{
    float halfThickness = lineThickness * 0.5f;
    
    // Get corners
    Vec2 const& topRightPoint = maxs;
    Vec2 const& bottomLeftPoint = mins;
    Vec2 bottomRightPoint = Vec2(maxs.x, mins.y);
    Vec2 topLeftPoint = Vec2(mins.x, maxs.y);

    out_verts.reserve(out_verts.size() + static_cast<size_t>(3 * 8));

    // Top and bottom
    AddVertsForLine2D(out_verts, topLeftPoint, topRightPoint, lineThickness, tint);
    AddVertsForLine2D(out_verts, bottomLeftPoint, bottomRightPoint, lineThickness, tint);

    // Left and right (dropped down by half thickness)
    Vec2 offset = Vec2(0.f, lineThickness);
    AddVertsForLine2D(out_verts, topLeftPoint - offset, bottomLeftPoint + offset, lineThickness, tint);
    AddVertsForLine2D(out_verts, topRightPoint - offset, bottomRightPoint + offset, lineThickness, tint);
}
