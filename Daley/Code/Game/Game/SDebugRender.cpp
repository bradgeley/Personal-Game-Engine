// Bradley Christensen - 2022-2025
#include "SDebugRender.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Startup()
{
    AddWriteAllDependencies();

    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_frameUntexVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();
    scDebug.m_frameDefaultFontVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();

    DevConsoleUtils::AddDevConsoleCommand("DebugRenderGrid", &SDebugRender::DebugRenderGrid);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{
	SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    Font* defaultFont = g_renderer->GetDefaultFont();

    VertexBuffer& untexturedVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameUntexVerts);
    VertexBuffer& defaultFontVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameDefaultFontVerts);

    #if defined(_DEBUG)
        //if (scDebug.m_debugRenderGrid)
        {
            g_renderer->BindTexture(nullptr);
            g_renderer->BindShader(nullptr);
            g_renderer->DrawVertexBuffer(scWorld.m_debugVBO);
        }
    #endif 

    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(scDebug.m_frameUntexVerts);

	defaultFont->SetRendererState();
    g_renderer->DrawVertexBuffer(scDebug.m_frameDefaultFontVerts);

	untexturedVerts.ClearVerts();
	defaultFontVerts.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Shutdown()
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();

    g_renderer->ReleaseVertexBuffer(scDebug.m_frameDefaultFontVerts);
    g_renderer->ReleaseVertexBuffer(scDebug.m_frameUntexVerts);

    DevConsoleUtils::RemoveDevConsoleCommand("DebugRenderGrid", &SDebugRender::DebugRenderGrid);
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderGrid(NamedProperties& args)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderGrid = !scDebug.m_debugRenderGrid;
    return false;
}
