// Bradley Christensen - 2022-2025
#include "SDebugOverlay.h"
#include "Chunk.h"
#include "CPlayerController.h"
#include "CTransform.h"
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

	Vec2 playerLocation = Vec2::ZeroVector;
	auto it = g_ecs->Iterate<CTransform, CPlayerController>(context);
	if (it.IsValid())
	{
		playerLocation = g_ecs->GetComponent<CTransform>(it)->m_pos;
	}

	// Player location + world coords
	WorldCoords playerWorldCoords = scWorld.GetWorldCoordsAtLocation(playerLocation);
	IntVec2 playerGlobalTileCoords = playerWorldCoords.GetGlobalTileCoords();
	std::string playerLocationString = StringUtils::StringF("Location (%.02f, %.02f)\nGlobal Coords (%i, %i)\nChunk Coords (%i, %i)\nLocal Coords (%i, %i)", 
															playerLocation.x, playerLocation.y, playerGlobalTileCoords.x, playerGlobalTileCoords.y, playerWorldCoords.m_chunkCoords.x, 
															playerWorldCoords.m_chunkCoords.y, playerWorldCoords.m_localTileCoords.x, playerWorldCoords.m_localTileCoords.y);

	font->AddVertsForAlignedText2D(fontVerts, screenBounds.GetBottomCenter(), Font::AlignTopCenter, 25.f, playerLocationString, Rgba8::White, 0.25f);

	// Tile data at player location
	TileGeneratedData tileData = Chunk::GenerateTileData(playerWorldCoords.GetGlobalTileCoords(), scWorld.m_worldSettings);
	std::string tileDataString = StringUtils::StringF("Seed: %i\n", scWorld.m_worldSettings.m_worldSeed);
	tileDataString += tileData.ToStringVerbose();
	font->AddVertsForAlignedText2D(fontVerts, screenBounds.GetTopLeft() + Vec2(10.f, -10.f), Font::AlignBottomRight, 25.f, tileDataString, Rgba8::White, 0.25f);

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
