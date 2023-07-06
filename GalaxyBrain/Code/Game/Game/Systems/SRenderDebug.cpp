// Bradley Christensen - 2023
#include "SRenderDebug.h"
#include "Game/Game/Singletons/SCDebug.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_Z_LAYERS = 2;



//----------------------------------------------------------------------------------------------------------------------
void SRenderDebug::Startup()
{

}



//----------------------------------------------------------------------------------------------------------------------
void SRenderDebug::Run(SystemContext const& context)
{
    auto& scDebug = *g_ecs->GetComponent<SCDebug>();

    g_renderer->SetModelConstants(ModelConstants());
    g_renderer->BindShader(0);
    g_renderer->BindTexture(0);
    g_renderer->DrawVertexBuffer(&scDebug.m_debugDrawVerts);

    scDebug.m_debugDrawVerts.ClearVerts();
}