// Bradley Christensen - 2022-2025
#include "SDebugRender.h"
#include "SCCamera.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "SCFlowField.h"
#include "FlowField.h"
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
    DevConsoleUtils::AddDevConsoleCommand("DebugRenderEdges", &SDebugRender::DebugRenderEdges);
    DevConsoleUtils::AddDevConsoleCommand("DebugTileTags", &SDebugRender::DebugTileTags, "Tag", DevConsoleArgType::String);
    DevConsoleUtils::AddDevConsoleCommand("DebugRenderCostField", &SDebugRender::DebugRenderCostField);
    DevConsoleUtils::AddDevConsoleCommand("DebugRenderDistanceField", &SDebugRender::DebugRenderDistanceField);
    DevConsoleUtils::AddDevConsoleCommand("DebugRenderFlowField", &SDebugRender::DebugRenderFlowField);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugRender::Run(SystemContext const& context)
{
	SCWorld const& world = g_ecs->GetSingleton<SCWorld>();
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    SCFlowField const& scFlowfield = g_ecs->GetSingleton<SCFlowField>();
    FlowField const& toGoalFlowField = scFlowfield.m_toGoalFlowField;
    Font* defaultFont = g_renderer->GetDefaultFont();

    VertexBuffer& untexturedVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameUntexVerts);
    VertexBuffer& defaultFontVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameDefaultFontVerts);

    if (scDebug.m_debugRenderGrid)
    {
        g_renderer->BindTexture(nullptr);
        g_renderer->BindShader(nullptr);
        g_renderer->DrawVertexBuffer(world.m_debugVBO);
    }

	SCCamera& scCamera = g_ecs->GetSingleton<SCCamera>();
    if (scDebug.m_debugRenderEdges)
    {
        scCamera.m_camera.SetOrthoBounds2D(world.GetPlayableWorldBounds());
    }
    else
    {
        scCamera.m_camera.SetOrthoBounds2D(world.GetVisibleWorldBounds());
    }

    if (scDebug.m_debugTileTags != 0)
    {
        world.ForEachPlayableWorldCoords([&](IntVec2 const& tileCoords, int)
        {
            Tile const& tile = world.m_tiles.Get(tileCoords);
            if ((tile.m_tags & scDebug.m_debugTileTags) != 0)
            {
                AABB2 tileBounds = world.GetTileBounds(tileCoords);
                Rgba8 highlightTint = Rgba8(0, 255, 255, 127);
                VertexUtils::AddVertsForAABB2(untexturedVerts, tileBounds, highlightTint);
            }
            return true; // keep iterating
		});
    }

    // Render Cost Field
    if (scDebug.m_debugRenderCostField)
    {
        for (int y = StaticWorldSettings::s_playableWorldBeginIndexY; y <= StaticWorldSettings::s_playableWorldEndIndexY; ++y)
        {
            for (int x = StaticWorldSettings::s_playableWorldBeginIndexX; x <= StaticWorldSettings::s_playableWorldEndIndexX; ++x)
            {
				IntVec2 tileCoords = IntVec2(x, y);
                if (!world.IsTileOnPath(tileCoords))
                {
                    continue;
                }

                AABB2 tileBounds = world.GetTileBounds(tileCoords);
                uint8_t cost = toGoalFlowField.m_costField.Get(x, y);
                float t = MathUtils::RangeMapClamped((float) cost, 0.f, 255.f, 0.f, 1.f);
                Rgba8 tint = Rgba8::Lerp(Rgba8(0, 255, 0, 100), Rgba8(255, 0, 0, 100), t);
                VertexUtils::AddVertsForAABB2(untexturedVerts, tileBounds, tint);
            }
        }
    }

    // Render Distance Field
    if (scDebug.m_debugRenderDistanceField)
    {
        for (int y = StaticWorldSettings::s_playableWorldBeginIndexY; y <= StaticWorldSettings::s_playableWorldEndIndexY; ++y)
        {
            for (int x = StaticWorldSettings::s_playableWorldBeginIndexX; x <= StaticWorldSettings::s_playableWorldEndIndexX; ++x)
            {
                IntVec2 tileCoords = IntVec2(x, y);
                if (!world.IsTileOnPath(tileCoords))
                {
                     continue;
				}

                AABB2 tileBounds = world.GetTileBounds(tileCoords);
                float distance = toGoalFlowField.m_distanceField.Get(x, y);

                float t = MathUtils::RangeMapClamped(distance, 0.f, 100.f, 0.f, 1.f);
                Rgba8 const& tint = Rgba8::Lerp(Rgba8::Green, Rgba8::Red, t);
                std::string distanceText = StringUtils::StringF("%.2f", distance);
                defaultFont->AddVertsForAlignedText2D(defaultFontVerts, tileBounds.GetCenter(), Vec2::ZeroVector, 0.5f, distanceText, tint);
            }
        }
    }

    // Render Flow Field
    if (scDebug.m_debugRenderFlowField)
    {
        for (int y = StaticWorldSettings::s_playableWorldBeginIndexY; y <= StaticWorldSettings::s_playableWorldEndIndexY; ++y)
        {
            for (int x = StaticWorldSettings::s_playableWorldBeginIndexX; x <= StaticWorldSettings::s_playableWorldEndIndexX; ++x)
            {
                IntVec2 tileCoords = IntVec2(x, y);
                if (!world.IsTileOnPath(tileCoords))
                {
                    continue;
                }

                AABB2 tileBounds = world.GetTileBounds(tileCoords);
                Vec2 gradient = toGoalFlowField.m_gradient.Get(x, y);

                VertexUtils::AddVertsForArrow2D(untexturedVerts, tileBounds.GetCenter() - gradient * 0.33f, tileBounds.GetCenter() + gradient * 0.33f, 0.075f, Rgba8::Yellow);
            }
        }
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
bool SDebugRender::DebugRenderEdges(NamedProperties& args)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderEdges = !scDebug.m_debugRenderEdges;
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
    else if (tag == "solid")
	{
		fakeTile.SetIsSolid(!fakeTile.IsSolid());
	}
    else if (tag == "opaque")
	{
		fakeTile.SetIsOpaque(!fakeTile.IsOpaque());
	}
    else if (tag == "goal")
    {
		fakeTile.SetIsGoal(!fakeTile.IsGoal());
    }
    else if (tag == "path")
    {
		fakeTile.SetIsPath(!fakeTile.IsPath());
    }
    else if (tag == "lightingDirty")
    {
        fakeTile.SetLightingDirty(!fakeTile.IsLightingDirty());
    }
    else if (tag == "vboDirty")
    {
        fakeTile.SetVertsDirty(!fakeTile.IsVertsDirty());
    }

	scDebug.m_debugTileTags = fakeTile.m_tags;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderCostField(NamedProperties& args)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderCostField = !scDebug.m_debugRenderCostField;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderDistanceField(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderDistanceField = !scDebug.m_debugRenderDistanceField;
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugRender::DebugRenderFlowField(NamedProperties&)
{
    SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
    scDebug.m_debugRenderFlowField = !scDebug.m_debugRenderFlowField;
    return false;
}
