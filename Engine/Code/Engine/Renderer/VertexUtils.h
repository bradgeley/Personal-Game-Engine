// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Math/AABB2.h"
#include <vector>



struct Rgba8;
struct Vec2;
struct AABB2;



//----------------------------------------------------------------------------------------------------------------------
// Vertex Utils
//
// Utility functions for pushing back verts to a list for rendering
//



//----------------------------------------------------------------------------------------------------------------------
// 2D
//
void AddVertsForLine2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& tint = Rgba8::White);
void AddVertsForArrow2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& tint = Rgba8::White);
void AddVertsForAABB2(std::vector<Vertex_PCU>& out_verts, AABB2 const& square, Rgba8 const& tint = Rgba8::White, AABB2 const& UVs = AABB2::ZeroToOne);
void AddVertsForRect2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& mins, Vec2 const& maxs, Rgba8 const& tint = Rgba8::White, AABB2 const& UVs = AABB2::ZeroToOne);
void AddVertsForWireBox2D(std::vector<Vertex_PCU>& out_verts, AABB2 const& box, float lineThickness, Rgba8 const& tint = Rgba8::White);
void AddVertsForWireBox2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& mins, Vec2 const& maxs, float lineThickness, Rgba8 const& tint = Rgba8::White);
void AddVertsForWireGrid(std::vector<Vertex_PCU>& out_verts, AABB2 const& boundingAABB, IntVec2 const& dims, float lineThickness, Rgba8 const& tint = Rgba8::White);
void AddVertsForGrid(std::vector<Vertex_PCU>& out_verts, AABB2 const& boundingAABB, IntVec2 const& dims, Rgba8 const& tint = Rgba8::Black);
void AddVertsForGrid(std::vector<Vertex_PCU>& out_verts, AABB2 const& boundingAABB, Rgba8 const* colorsRowMajor, IntVec2 const& dims, Rgba8 const& gridLinesTint = Rgba8::Black);
void AddVertsForDisc2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& center, float radius, int numSides, Rgba8 const& tint = Rgba8::White, AABB2 const& UVs = AABB2::ZeroToOne);
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& out_verts, Vec2 const& start, Vec2 const& end, float radius, Rgba8 const& tint = Rgba8::White);