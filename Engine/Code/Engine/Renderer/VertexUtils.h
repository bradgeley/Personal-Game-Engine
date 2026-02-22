// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/AABB2.h"
#include "Engine/Renderer/Rgba8.h"



struct Vec2;
struct AABB2;
struct Vertex_PCU;
class VertexBuffer;



//----------------------------------------------------------------------------------------------------------------------
// Vertex Utils
//
// Utility functions for pushing back Vertex_PCU's to a list for rendering. Todo: make work for other vert types, if possible?
//
namespace VertexUtils
{
	//----------------------------------------------------------------------------------------------------------------------
	// 2D
	//
	void AddVertsForLine2D(VertexBuffer& out_verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& tint = Rgba8::White);
	void AddVertsForArrow2D(VertexBuffer& out_verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& tint = Rgba8::White);
	void AddVertsForAABB2(VertexBuffer& out_verts, AABB2 const& square, Rgba8 const& tint = Rgba8::White, AABB2 const& UVs = AABB2::ZeroToOne);
	void AddVertsForRect2D(VertexBuffer& out_verts, Vec2 const& mins, Vec2 const& maxs, Rgba8 const& tint = Rgba8::White, AABB2 const& UVs = AABB2::ZeroToOne, float z = 0.f);
	void AddVertsForWireBox2D(VertexBuffer& out_verts, AABB2 const& box, float lineThickness, Rgba8 const& tint = Rgba8::White);
	void AddVertsForWireBox2D(VertexBuffer& out_verts, Vec2 const& mins, Vec2 const& maxs, float lineThickness, Rgba8 const& tint = Rgba8::White);
	void AddVertsForWireGrid(VertexBuffer& out_verts, AABB2 const& boundingAABB, IntVec2 const& dims, float lineThickness, Rgba8 const& tint = Rgba8::White);
	void AddVertsForGrid(VertexBuffer& out_verts, AABB2 const& boundingAABB, IntVec2 const& dims, Rgba8 const& tint = Rgba8::Black);
	void AddVertsForGrid(VertexBuffer& out_verts, AABB2 const& boundingAABB, Rgba8 const* colorsRowMajor, IntVec2 const& dims, Rgba8 const& gridLinesTint = Rgba8::Black);
	void AddVertsForDisc2D(VertexBuffer& out_verts, Vec2 const& center, float radius, int numSides, Rgba8 const& tint = Rgba8::White, AABB2 const& UVs = AABB2::ZeroToOne);
	void AddVertsForWireDisc2D(VertexBuffer& out_verts, Vec2 const& center, float radius, float thickness, int numSides, Rgba8 const& tint = Rgba8::White);
	void AddVertsForCapsule2D(VertexBuffer& out_verts, Vec2 const& start, Vec2 const& end, float radius, Rgba8 const& tint = Rgba8::White);
	void AddVertsForWireMesh2D(VertexBuffer& out_verts, VertexBuffer const& triangles, float thickness, Rgba8 const& tint = Rgba8::White);

	int WriteVertsForRect2D(Vertex_PCU* writeLocation, Vec2 const& mins, Vec2 const& maxs, Rgba8 const& tint = Rgba8::White, AABB2 const& UVs = AABB2::ZeroToOne, float z = 0.f);
}

