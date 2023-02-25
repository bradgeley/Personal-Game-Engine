// Bradley Christensen - 2022-2023
#include "Game.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"



void Game::Startup()
{
    m_camera = Camera(Vec3(-1000.f,-1000.f,-1.f), Vec3(1000.f, 1000.f, 1.f));
    
    m_vertexBuffer = new VertexBuffer();
    m_vertexBuffer->Initialize();

    std::vector<Vertex_PCU> verts;
    verts.reserve(9);
    verts.emplace(verts.end(), Vec3(0.f,0.f,1.f), Rgba8::RED);
    verts.emplace(verts.end(), Vec3(1000,0.f,1.f), Rgba8::RED);
    verts.emplace(verts.end(), Vec3(0.f,1000.f,1.f), Rgba8::RED);
    
    verts.emplace(verts.end(), Vec3(0.f,0.f,1.f), Rgba8::RED);
    verts.emplace(verts.end(), Vec3(-1000.f,0.f,1.f), Rgba8::RED);
    verts.emplace(verts.end(), Vec3(0.f,-1000.f,1.f), Rgba8::RED);
    
    verts.emplace(verts.end(), Vec3(-0.f,-1.f,1.f), Rgba8::RED);
    verts.emplace(verts.end(), Vec3(-1.f,0.f,1.f), Rgba8::RED);
    verts.emplace(verts.end(), Vec3(1.f,0.f,1.f), Rgba8::RED);
    
    m_vertexBuffer->AddVerts(verts);
    m_vertexBuffer->UpdateGPUBuffer();
}



Mat44 g_testModelMatrix = Mat44();
Rgba8 g_testTint;



void Game::Update(float deltaSeconds)
{
    static float t = 0.f;
    t += deltaSeconds;
    g_testTint = Rgba8::Lerp(Rgba8::WHITE, Rgba8::BLACK, t);

    g_testModelMatrix.AppendZRotation(deltaSeconds * 90.f);
}



void Game::Render() const
{
    g_renderer->ClearScreen(Rgba8::MAGENTA);
    g_renderer->BeginCamera(m_camera);
    g_renderer->SetModelMatrix(g_testModelMatrix);
    g_renderer->SetModelTint(g_testTint);
    g_renderer->DrawVertexBuffer(m_vertexBuffer);
}
