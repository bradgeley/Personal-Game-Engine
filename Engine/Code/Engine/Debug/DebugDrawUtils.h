// Bradley Christensen - 2022-2023
#pragma once
#include "Engine/Renderer/Vertex_PCU.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// Debug Draw Utils
//
void DebugDrawLine2D(Vec2 const& start, Vec2 const& end, float thickness = 1.f, Rgba8 tint = Rgba8::Red);
void DebugDrawMesh2D(std::vector<Vertex_PCU> const& triangles, float thickness = 1.f, Rgba8 tint = Rgba8::Red);