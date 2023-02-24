// Bradley Christensen - 2022
#include "Game.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"



void Game::Startup()
{
    m_camera = Camera(Vec3(-1000.f,-1000.f,-1.f), Vec3(1000.f, 1000.f, 1.f));
    
    m_vertexBuffer = new VertexBuffer();
    m_vertexBuffer->Initialize();

    std::vector<Vertex_PCU> verts;  
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



void Game::Update(float deltaSeconds)
{

}



void Game::Render() const
{
    g_renderer->ClearScreen(Rgba8::MAGENTA);
    g_renderer->BeginCamera(m_camera);
    g_renderer->DrawVertexBuffer(m_vertexBuffer);
}
