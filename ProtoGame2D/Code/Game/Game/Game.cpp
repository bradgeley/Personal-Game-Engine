// Bradley Christensen - 2022-2023
#include "Game.h"

#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"


Mat44 g_testModelMatrix = Mat44();
Mat44 g_renderModelMatrix = Mat44();
Rgba8 g_testTint;



void Game::Startup()
{
    m_camera = Camera(Vec3(-1000.f,-1000.f,-1000.f), Vec3(1000.f, 1000.f, 1000.f));
    
    m_spinningTextureVerts = new VertexBuffer();
    m_spinningTextureVerts->Initialize();

    auto& spinningTexVerts = m_spinningTextureVerts->GetMutableVerts();
    AddVertsForSquare2D(spinningTexVerts, Vec2(-50.f, -50.f), Vec2(50.f,50.f));
    m_spinningTextureVerts->AddVerts(spinningTexVerts);

    m_texture = new Texture(); 
    m_texture->LoadFromImageFile("Data/Images/TestUV.png");

    m_staticGeometryVerts = new VertexBuffer();
    m_staticGeometryVerts->Initialize();

    auto& staticGeoVerts = m_staticGeometryVerts->GetMutableVerts();
    AddVertsForLine2D(staticGeoVerts, Vec2(-100.f, -100.f), Vec2(100.f, 100.f), 5.f);
    AddVertsForWireBox2D(staticGeoVerts, Vec2(-400.f, -400.f), Vec2(-250.f, -250.f), 5.f);
}



void Game::Update(float deltaSeconds)
{
    static float t = 0.f;
    t += deltaSeconds;
    g_testTint = Rgba8::Lerp(Rgba8::WHITE, Rgba8::BLACK, t);

    g_testModelMatrix.AppendZRotation(deltaSeconds * 90.f);

    float scale = 1.f + 0.5f * SinRadians(t);
    g_renderModelMatrix = g_testModelMatrix.GetAppended(Mat44::CreateUniformScale2D(scale));
}



void Game::Render() const
{
    g_renderer->BeginCamera(m_camera); // reset renderer state

    // Clear screen first
    g_renderer->ClearScreen(Rgba8::MAGENTA);
    
    // Then draw static geo
    g_renderer->DrawVertexBuffer(m_staticGeometryVerts);

    // Then moving things
    g_renderer->SetModelMatrix(g_renderModelMatrix);
    g_renderer->SetModelTint(g_testTint);
    g_renderer->BindTexture(m_texture);
    g_renderer->DrawVertexBuffer(m_spinningTextureVerts);
    
}



void Game::Shutdown()
{
    delete m_texture;
    delete m_staticGeometryVerts;
    delete m_spinningTextureVerts;
}
