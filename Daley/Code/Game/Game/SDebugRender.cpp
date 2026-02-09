// Bradley Christensen - 2022-2025
#include "SDebugRender.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/DataStructures/NamedProperties.h"
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
    DevConsoleUtils::AddDevConsoleCommand("DebugTileTags", &SDebugRender::DebugTileTags, "Tag", DevConsoleArgType::String);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{
	SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    Font* defaultFont = g_renderer->GetDefaultFont();

    VertexBuffer& untexturedVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameUntexVerts);
    VertexBuffer& defaultFontVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameDefaultFontVerts);

    if (scDebug.m_debugRenderGrid)
    {
        g_renderer->BindTexture(nullptr);
        g_renderer->BindShader(nullptr);
        g_renderer->DrawVertexBuffer(scWorld.m_debugVBO);
    }

    if (scDebug.m_debugTileTags != 0)
    {
        scWorld.ForEachVisibleWorldCoords([&](IntVec2 const& tileCoords, int)
        {
            Tile const& tile = scWorld.m_tiles.Get(tileCoords);
            if ((tile.m_tags & scDebug.m_debugTileTags) != 0)
            {
                AABB2 tileBounds = scWorld.GetTileBounds(tileCoords);
                Rgba8 highlightTint = Rgba8(0, 255, 255, 127);
                VertexUtils::AddVertsForAABB2(untexturedVerts, tileBounds, highlightTint);
            }
            return true; // keep iterating
		});
    }

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
    DevConsoleUtils::RemoveDevConsoleCommand("DebugTileTags", &SDebugRender::DebugTileTags);
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderGrid(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderGrid = !scDebug.m_debugRenderGrid;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugTileTags(NamedProperties& args)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	Name tag = args.Get("Tag", "");

    Tile fakeTile;
	fakeTile.m_tags = scDebug.m_debugTileTags;
    
    if (tag == "visible")
	{
        fakeTile.SetIsVisible(!fakeTile.IsVisible());
	}
    if (tag == "solid")
	{
		fakeTile.SetIsSolid(!fakeTile.IsSolid());
	}
	if (tag == "opaque")
	{
		fakeTile.SetIsOpaque(!fakeTile.IsOpaque());
	}
    if (tag == "goal")
    {
		fakeTile.SetIsGoal(!fakeTile.IsGoal());
    }
    if (tag == "lightingDirty")
    {
        fakeTile.SetLightingDirty(!fakeTile.IsLightingDirty());
    }
    if (tag == "vboDirty")
    {
        fakeTile.SetVertsDirty(!fakeTile.IsVertsDirty());
    }

	scDebug.m_debugTileTags = fakeTile.m_tags;
    return false;
}
