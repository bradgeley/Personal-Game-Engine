// Bradley Christensen - 2022-2023
#include "Engine/Renderer/DebugDrawUtils.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void DebugDrawMesh2D(std::vector<Vertex_PCU> const& triangles, float thickness, Rgba8 tint)
{
    // For each triangle, draw 18 verts, so for each vert in "triangles" we push 6 verts
    VertexBuffer vbo;
    auto& verts = vbo.GetMutableVerts();
    verts.reserve(triangles.size() * 6);
    
    for (int i = 0; i < (int) triangles.size(); i += 3)
    {
        Vertex_PCU const& v1 = triangles[i];
        Vertex_PCU const& v2 = triangles[i+1];
        Vertex_PCU const& v3 = triangles[i+2];
        
        AddVertsForLine2D(verts, Vec2(v1.pos), Vec2(v2.pos), thickness, tint);
        AddVertsForLine2D(verts, Vec2(v2.pos), Vec2(v3.pos), thickness, tint);
        AddVertsForLine2D(verts, Vec2(v3.pos), Vec2(v1.pos), thickness, tint);
    }

    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(&vbo);
}
