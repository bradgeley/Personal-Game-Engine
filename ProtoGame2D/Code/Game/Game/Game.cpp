// Bradley Christensen - 2022-2023
#include "Game.h"

#include "GameCommon.h"
#include "WindowsApplication.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Input/InputUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/DebugDrawUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



Mat44 g_testModelMatrix = Mat44();
Mat44 g_renderModelMatrix = Mat44();
Mat44 g_helloWorldModelMatrix = Mat44();
float g_helloWorldScale = 0.f;
Rgba8 g_testTint;



void Game::Startup()
{
    m_camera = Camera(Vec3(-2000.f,-1000.f,-1000.f), Vec3(2000.f, 1000.f, 1000.f));
    
    m_spinningTextureVerts = new VertexBuffer();

    auto& spinningTexVerts = m_spinningTextureVerts->GetMutableVerts();
    AddVertsForRect2D(spinningTexVerts, Vec2(-50.f, -50.f), Vec2(50.f,50.f));
    m_spinningTextureVerts->AddVerts(spinningTexVerts);

    m_texture = new Texture(); 
    m_texture->LoadFromImageFile("Data/Images/TestUV.png");

    m_staticGeometryVerts = new VertexBuffer();

    auto& staticGeoVerts = m_staticGeometryVerts->GetMutableVerts();
    AddVertsForLine2D(staticGeoVerts, Vec2(-200.f, -200.f), Vec2(0.f, 0.f), 25.f);
    AddVertsForWireBox2D(staticGeoVerts, Vec2(-900.f, -900.f), Vec2(-200.f, -200.f), 50.f);

    m_textVerts = new VertexBuffer();
    auto& textVerts = m_textVerts->GetMutableVerts();
    Font* font = g_renderer->GetDefaultFont();
    if (font)
    {
        font->AddVertsForText2D(textVerts, Vec2(-970.f, 300.f), 400.f, "Hello, soBali!");
    }
}



void Game::Update(float deltaSeconds)
{
    static float t = 0.f;
    t += deltaSeconds;

    if (g_input->WasKeyJustReleased(KeyCode::ESCAPE))
    {
        g_theApp->Quit();
    }

    if (g_input->IsMouseButtonDown(0))
    {
        g_testTint = Rgba8::LightBlue;
    }
    else
    {
        g_testTint = Rgba8::Lerp(Rgba8::White, Rgba8::Black, t);
    }

    g_testModelMatrix.AppendZRotation(deltaSeconds * 90.f);

    float scale = 1.f + 0.5f * SinRadians(t);
    g_renderModelMatrix = g_testModelMatrix.GetAppended(Mat44::CreateUniformScale2D(scale));

    if (g_input->IsKeyDown('B'))
    {
        g_helloWorldScale = 0.5f;
    }
    else
    {
        g_helloWorldScale = (1.f + 0.25f * SinRadians(t));
    }
    g_helloWorldModelMatrix = Mat44();
    g_helloWorldModelMatrix.AppendUniformScale2D(g_helloWorldScale);

    if (g_input->IsMouseButtonDown(0))
    {
        auto& verts = m_staticGeometryVerts->GetMutableVerts();
        Vec2 mouseRelativePos = g_input->GetMouseClientRelativePosition();
        Vec2 worldPos = m_camera.ScreenToWorldOrtho(mouseRelativePos);
        AddVertsForLine2D(verts, Vec2::ZeroVector, worldPos, 5.f);
    }
}

 

void Game::Render() const
{
    g_renderer->BeginCamera(m_camera); // reset renderer state

    // Clear screen first
    g_renderer->ClearScreen(Rgba8::Magenta);
    
    // Then draw static geo
    g_renderer->DrawVertexBuffer(m_staticGeometryVerts);
    DebugDrawMesh2D(m_staticGeometryVerts->GetVerts(), 5.f);

    // Then moving things
    g_renderer->SetModelMatrix(g_renderModelMatrix);
    g_renderer->SetModelTint(g_testTint);
    g_renderer->BindTexture(m_texture);
    g_renderer->DrawVertexBuffer(m_spinningTextureVerts);
    DebugDrawMesh2D(m_spinningTextureVerts->GetVerts(), 5.f);
    
    // Then text
    Font* font = g_renderer->GetDefaultFont();
    if (font)
	{
		font->SetRendererState();
		g_renderer->SetModelMatrix(g_helloWorldModelMatrix);
		g_renderer->DrawVertexBuffer(m_textVerts);
		DebugDrawMesh2D(m_textVerts->GetVerts(), 5.f);
    }
}



void Game::Shutdown()
{
    delete m_texture;
    delete m_staticGeometryVerts;
    delete m_spinningTextureVerts;
    delete m_textVerts;
}
