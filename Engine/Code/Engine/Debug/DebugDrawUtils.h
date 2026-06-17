// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Vertex_PCU.h"
#include <vector>



class Renderer;



//----------------------------------------------------------------------------------------------------------------------
// Debug Draw Utils
//
void DebugDrawMesh2D(Renderer& renderer, std::vector<Vertex_PCU> const& triangles, float thickness = 1.f, Rgba8 tint = Rgba8::Red);