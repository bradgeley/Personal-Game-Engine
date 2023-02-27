// Bradley Christensen - 2022-2023
#include "Game.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexBuffer.h"



void Game::Startup()
{
    m_camera = Camera(Vec3(-1000.f,-1000.f,-1.f), Vec3(1000.f, 1000.f, 1.f));
    
    m_vertexBuffer = new VertexBuffer();
    m_vertexBuffer->Initialize();

    Rgba8 tint = Rgba8::WHITE;
    Vec3 bottomRightPoint = Vec3(500.f, -500.f, 1.f);
    Vec2 bottomRightUVs = Vec2(1.f, 0.f);
    Vec3 topRightPoint = Vec3(500.f, 500.f, 1.f);
    Vec2 topRightUVs = Vec2(1.f, 1.f);
    Vec3 bottomLeftPoint = Vec3(-500.f, -500.f, 1.f);
    Vec2 bottomLeftUVs = Vec2(0.f, 0.f);
    Vec3 topLeftPoint = Vec3(-500.f, 500.f, 1.f);
    Vec2 topLeftUVs = Vec2(0.f, 1.f);

    std::vector<Vertex_PCU> verts;
    verts.reserve(9);
    verts.emplace(verts.end(), bottomLeftPoint, tint, bottomLeftUVs);
    verts.emplace(verts.end(), bottomRightPoint, tint, bottomRightUVs);
    verts.emplace(verts.end(), topRightPoint, tint, topRightUVs);
    
    verts.emplace(verts.end(), topRightPoint, tint, topRightUVs);
    verts.emplace(verts.end(), topLeftPoint, tint, topLeftUVs);
    verts.emplace(verts.end(), bottomLeftPoint, tint, bottomLeftUVs);
    m_vertexBuffer->AddVerts(verts);
    m_vertexBuffer->UpdateGPUBuffer();

    m_texture = new Texture();
    m_texture->LoadFromImageFile("Data/Images/TestUV.png");
    g_renderer->BindTexture(m_texture);
}



Mat44 g_testModelMatrix = Mat44();
Rgba8 g_testTint;



void Game::Update(float deltaSeconds)
{
    static float t = 0.f;
    t += deltaSeconds;
    g_testTint = Rgba8::Lerp(Rgba8::WHITE, Rgba8::BLACK, t);

    g_testModelMatrix.AppendZRotation(deltaSeconds * 90.f);
    g_testModelMatrix.AppendUniformScale(0.99f);
}



void Game::Render() const
{
    g_renderer->ClearScreen(Rgba8::MAGENTA);
    g_renderer->BeginCamera(m_camera);
    g_renderer->SetModelMatrix(g_testModelMatrix);
    g_renderer->SetModelTint(g_testTint);
    g_renderer->DrawVertexBuffer(m_vertexBuffer);
}
