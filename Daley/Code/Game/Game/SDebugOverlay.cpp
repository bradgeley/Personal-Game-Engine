// Bradley Christensen - 2022-2026
#include "SDebugOverlay.h"
#include "CCollision.h"
#include "CEntityDebug.h"
#include "CHealth.h"
#include "CTags.h"
#include "CTransform.h"
#include "CWeapon.h"
#include "SCCamera.h"
#include "SCDebug.h"
#include "SCWaves.h"
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
void SDebugOverlay::Run(SystemContext const&)
{
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	SCCamera& worldCamera = g_ecs->GetSingleton<SCCamera>();

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

	Vec2 topLeft = screenBounds.GetTopLeft();
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(10.f, -10.f), Vec2(1.f, -1.f), 25.f, "Debug Overlay Enabled", Rgba8::White);

	int totalEntities = g_ecs->NumEntities();
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(10.f, -40.f), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Total Entities: %d", totalEntities), Rgba8::White);

	// Wave spawner info
	SCWaves const& waves = g_ecs->GetSingleton<SCWaves>();
	Vec2 wavesTopLeft = screenBounds.maxs - Vec2(350.f, 10.f);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft, Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Waves: %s", waves.m_wavesStarted ? (waves.m_wavesFinished ? "Finished" : "In Progress") : "Not Started"), Rgba8::White);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 30.f), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Current Wave: %d/%d", waves.m_currentWaveIndex, waves.m_waves.size()), Rgba8::White);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 60.f), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Time Until Next Wave: %.1f", waves.m_waveTimer.GetRemainingSeconds()), Rgba8::Yellow);
	for (int streamIndex = 0; streamIndex < (int) waves.m_activeStreams.size(); ++streamIndex)
	{
		ActiveStream const& activeStream = waves.m_activeStreams[streamIndex];
		font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 90.f + 30.f * (float) streamIndex), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Active Stream: %s (%d/%d)", activeStream.m_entityStream.m_entityName.ToCStr(), activeStream.m_numSpawned, activeStream.m_entityStream.m_numEntities), Rgba8::White);
	}

	// Show tower weapon information for hovered tower
	auto& tagsStorage = g_ecs->GetArrayStorage<CTags>();
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& collStorage = g_ecs->GetArrayStorage<CCollision>();
	auto& debugStorage = g_ecs->GetArrayStorage<CEntityDebug>();
	auto& weaponStorage = g_ecs->GetMapStorage<CWeapon>();
	for (auto it = g_ecs->IterateAll<CTags, CTransform, CEntityDebug>(); it.IsValid(); ++it)
	{
		CTags const& tags = *tagsStorage.Get(it);
		if (tags.HasTag("Tower") || tags.HasTag("enemy"))
		{
			CTransform const& transform = *transStorage.Get(it);
			CCollision const* collision = collStorage.Get(it);
			float radius = collision ? collision->m_radius : 2.f;
			if (scDebug.m_debugMouseWorldLocation.GetDistanceSquaredTo(transform.m_pos) < (radius * radius))
			{
				CEntityDebug const& debug = *debugStorage.Get(it);
				Vec2 screenPos = worldCamera.m_camera.WorldToScreenRelativeOrtho(transform.m_pos + Vec2(radius, radius)) * screenCamera.GetOrthoDimensions2D();
				Vec2 cardMins = screenPos;
				Vec2 cardDims = Vec2(300.f, 300.f);
				AABB2 informationCardBounds = AABB2(cardMins, cardMins + cardDims);
				VertexUtils::AddVertsForAABB2(untexVerts, informationCardBounds, Rgba8::DarkGray);
				VertexUtils::AddVertsForWireBox2D(untexVerts, informationCardBounds, 5.f, Rgba8::Black);
				font->AddVertsForAlignedText2D(fontVerts, informationCardBounds.GetTopLeft() + Vec2(5.f, -5.f), Vec2(1.f, -1.f), 33.f, debug.m_defName.ToString(), Rgba8::White);

				std::string debugString;

				CWeapon const* weaponComp = weaponStorage.Get(it);
				if (weaponComp)
				{
					for (auto& weapon : weaponComp->m_weapons)
					{
						weapon->GetDebugString(debugString);
					}
				}

				CHealth const* health = g_ecs->GetComponent<CHealth>(it);
				if (health)
				{
					debugString += StringUtils::StringF("Health: %.1f / %.1f", health->m_currentHealth, health->m_maxHealth);
				}

				font->AddVertsForAlignedText2D(fontVerts, informationCardBounds.GetTopLeft() + Vec2(5.f, -50.f), Vec2(1.f, -1.f), 25.f, debugString, Rgba8::White, 0.33f);
				break;
			}
		}
	}

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
