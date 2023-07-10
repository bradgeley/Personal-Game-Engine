// Bradley Christensen - 2023
#include "SRenderDebug.h"
#include "Game/SCDebug.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_Z_LAYERS = 2;



//----------------------------------------------------------------------------------------------------------------------
void SRenderDebug::Startup()
{

}



//----------------------------------------------------------------------------------------------------------------------
void SRenderDebug::Run(SystemContext const& context)
{
    auto& scDebug = g_ecs->GetSingleton<SCDebug>();

    g_renderer->SetModelConstants(ModelConstants());
    g_renderer->BindShader(0);
    g_renderer->BindTexture(0);
    g_renderer->DrawVertexBuffer(&scDebug.m_debugDrawVerts);

    scDebug.m_debugDrawVerts.ClearVerts();

    float windowAspect = g_window->GetAspect();
    Camera debugCamera(Vec3(0.f, 0.f, 0.f), Vec3(windowAspect, 1.f, 1.f));
    g_renderer->BeginWindow(g_window);
    g_renderer->BeginCamera(debugCamera);

    VertexBuffer textVerts;
    auto font = g_renderer->GetDefaultFont();
    float fps = (1.f / context.m_deltaSeconds);
    font->AddVertsForAlignedText2D(textVerts.GetMutableVerts(), Vec2(0.f, 1.f), Vec2(1.f, -1.f), 0.02f, StringF("FPS: %.3f", fps), Rgba8::White);

    font->SetRendererState();
    g_renderer->DrawVertexBuffer(&textVerts);
}