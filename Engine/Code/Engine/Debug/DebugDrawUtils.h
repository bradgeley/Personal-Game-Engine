// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Vertex_PCU.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// Debug Draw Utils
//
void DebugDrawMesh2D(std::vector<Vertex_PCU> const& triangles, float thickness = 1.f, Rgba8 tint = Rgba8::Red);