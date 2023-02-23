// Bradley Christensen - 2022
#include "Game.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"



void Game::Startup()
{
    m_vertexBuffer = new VertexBuffer();
    m_vertexBuffer->Initialize();

    std::vector<Vertex_PCU> verts;
    verts.emplace(verts.end(), Vec3(0.f,0.f,1.f), Rgba8::RED);
    verts.emplace(verts.end(), Vec3(1.f,0.f,1.f), Rgba8::BLUE);
    verts.emplace(verts.end(), Vec3(0.5f,1.f,1.f), Rgba8::GREEN);
    
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
