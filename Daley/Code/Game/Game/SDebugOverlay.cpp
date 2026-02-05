// Bradley Christensen - 2022-2025
#include "SDebugOverlay.h"
#include "SCDebug.h"
#include "SCWorld.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugOverlay::Startup()
{
    AddWriteAllDependencies();

	DevConsoleUtils::AddDevConsoleCommand("ToggleDebugOverlay", &SDebugOverlay::ToggleDebugOverlay);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugOverlay::Run(SystemContext const& context)
{
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();

	if (g_input->IsKeyDown(KeyCode::Ctrl) && g_input->WasKeyJustPressed('D'))
	{
		scDebug.m_debugOverlayEnabled = !scDebug.m_debugOverlayEnabled;
	}

	if (!scDebug.m_debugOverlayEnabled)
	{
		return;
	}

	VertexBuffer& untexVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameUntexVerts);
	VertexBuffer& fontVerts = *g_renderer->GetVertexBuffer(scDebug.m_frameDefaultFontVerts);
	Font* font = g_renderer->GetDefaultFont();

	Camera screenCamera;
	IntVec2 resolution = g_window->GetRenderResolution();
	AABB2 screenBounds = AABB2(0.f, 0.f, (float) resolution.x, (float) resolution.y);
	screenCamera.SetOrthoBounds2D(screenBounds);

	// Add transparent black background
	VertexUtils::AddVertsForAABB2(untexVerts, screenBounds, Rgba8(0, 0, 0, 120), AABB2::ZeroToOne);

	// ...
	// Custom rendering stuff
	// ...

	g_renderer->BeginCamera(&screenCamera);

	// Render->clear debug verts 
	g_renderer->BindTexture(nullptr);
	g_renderer->BindShader(nullptr);
	g_renderer->DrawVertexBuffer(untexVerts);
	untexVerts.ClearVerts();

	// Render->clear debug text verts 
	font->SetRendererState();
	g_renderer->DrawVertexBuffer(fontVerts);
	fontVerts.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugOverlay::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("ToggleDebugOverlay", &SDebugOverlay::ToggleDebugOverlay);
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugOverlay::ToggleDebugOverlay(NamedProperties&)
{
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	scDebug.m_debugOverlayEnabled = !scDebug.m_debugOverlayEnabled;
	return false;
}
