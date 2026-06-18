// Bradley Christensen - 2022-2026
#include "Engine/Debug/DebugDrawUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void DebugDrawMesh2D(Renderer& renderer, std::vector<Vertex_PCU> const& triangles, float thickness, Rgba8 tint)
{
    VertexBufferID id = renderer.MakeVertexBuffer<Vertex_PCU>();
    VertexBuffer& vbo = *renderer.GetVertexBuffer(id);
    
    for (size_t i = 0; i < triangles.size(); i += 3)
    {
        Vertex_PCU const& v1 = triangles[i];
        Vertex_PCU const& v2 = triangles[i+1];
        Vertex_PCU const& v3 = triangles[i+2];
        
        VertexUtils::AddVertsForLine2D(vbo, Vec2(v1.pos), Vec2(v2.pos), thickness, tint);
        VertexUtils::AddVertsForLine2D(vbo, Vec2(v2.pos), Vec2(v3.pos), thickness, tint);
        VertexUtils::AddVertsForLine2D(vbo, Vec2(v3.pos), Vec2(v1.pos), thickness, tint);
    }

    renderer.BindTexture();
    renderer.BindShader();
    renderer.DrawVertexBuffer(vbo);

    renderer.ReleaseVertexBuffer(id);
}
