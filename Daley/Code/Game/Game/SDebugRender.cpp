// Bradley Christensen - 2022-2025
#include "SDebugRender.h"
#include "SCDebug.h"
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
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{

}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Shutdown()
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();

    g_renderer->ReleaseVertexBuffer(scDebug.m_frameDefaultFontVerts);
    g_renderer->ReleaseVertexBuffer(scDebug.m_frameUntexVerts);
}