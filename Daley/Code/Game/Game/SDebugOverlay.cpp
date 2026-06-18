// Bradley Christensen - 2022-2026
#include "SDebugOverlay.h"
#include "CAbility.h"
#include "CCollision.h"
#include "CEntityDebug.h"
#include "CHealth.h"
#include "CMovement.h"
#include "CProjectile.h"
#include "CTags.h"
#include "CTime.h"
#include "CTransform.h"
#include "SCCamera.h"
#include "SCDebug.h"
#include "SCFlowField.h"
#include "SCInputSystem.h"
#include "SCRenderer.h"
#include "SCWaves.h"
#include "SCWindow.h"
#include "Engine/Assets/Font.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/Camera.h"
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
void SDebugOverlay::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("ToggleDebugOverlay", &SDebugOverlay::ToggleDebugOverlay);
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugOverlay::Run(SystemContext const& context) const
{
	// Write Dependencies
	SCDebug& scDebug = context.GetSingleton<SCDebug>();
	Renderer& renderer = *context.GetSingleton<SCRenderer>().GetRenderer();

	// Read Dependencies
	SCCamera const& worldCamera = context.GetSingletonConst<SCCamera>();
	SCWaves const& waves = context.GetSingletonConst<SCWaves>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();
	auto& tagsStorage = context.GetArrayStorageConst<CTags>();
	auto& transStorage = context.GetArrayStorageConst<CTransform>();
	auto& debugStorage = context.GetArrayStorageConst<CEntityDebug>();
	auto& abilityStorage = context.GetMapStorageConst<CAbility>();
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	auto& timeStorage = context.GetArrayStorageConst<CTime>();
	auto& movementStorage = context.GetArrayStorageConst<CMovement>();
	auto& projectileStorage = context.GetMapStorageConst<CProjectile>();
	InputSystem const& input = *context.GetSingletonConst<SCInputSystem>().GetInputSystem();
	Window const& window = *context.GetSingletonConst<SCWindow>().GetWindow();

	if (input.IsKeyDown(KeyCode::Ctrl) && input.WasKeyJustPressed('D'))
	{
		scDebug.m_debugOverlayEnabled = !scDebug.m_debugOverlayEnabled;
	}

	if (!scDebug.m_debugOverlayEnabled)
	{
		return;
	}

	VertexBuffer& untexVerts = *renderer.GetVertexBuffer(scDebug.m_frameUntexVerts);
	VertexBuffer& fontVerts = *renderer.GetVertexBuffer(scDebug.m_frameDefaultFontVerts);
	Font const* font = renderer.GetDefaultFont();
	if (!font)
	{
		return;
	}

	Camera screenCamera;
	IntVec2 resolution = window.GetRenderResolution();
	AABB2 screenBounds = AABB2(0.f, 0.f, (float) resolution.x, (float) resolution.y);
	screenCamera.SetOrthoBounds2D(screenBounds);

	// Add transparent black background
	VertexUtils::AddVertsForAABB2(untexVerts, screenBounds, Rgba8(0, 0, 0, 120), AABB2::ZeroToOne);

	Vec2 topLeft = screenBounds.GetTopLeft();
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(10.f, -10.f), Vec2(1.f, -1.f), 25.f, "Debug Overlay Enabled", Rgba8::White);

	int totalEntities = context.NumEntities();
	font->AddVertsForAlignedText2D(fontVerts, topLeft + Vec2(10.f, -40.f), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Total Entities: %d", totalEntities), Rgba8::White);

	// Wave spawner info
	Vec2 wavesTopLeft = screenBounds.maxs - Vec2(350.f, 10.f);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft, Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Waves: %s", waves.m_wavesStarted ? (waves.m_wavesFinished ? "Finished" : "In Progress") : "Not Started"), Rgba8::White);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 30.f), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Current Wave: %d/%d", waves.m_currentWaveIndex, waves.m_waves.size()), Rgba8::White);
	font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 60.f), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Time Until Next Wave: %.1f", waves.m_waveTimer.GetRemainingSeconds()), Rgba8::Yellow);
	for (int streamIndex = 0; streamIndex < (int) waves.m_activeStreams.size(); ++streamIndex)
	{
		ActiveWaveStream const& activeStream = waves.m_activeStreams[streamIndex];
		font->AddVertsForAlignedText2D(fontVerts, wavesTopLeft - Vec2(0.f, 90.f + 30.f * (float) streamIndex), Vec2(1.f, -1.f), 25.f, StringUtils::StringF("Active Stream: %s (%d/%d)", activeStream.m_entityStream.m_entityName.ToCStr(), activeStream.m_numSpawned, activeStream.m_entityStream.m_numEntities), Rgba8::White);
	}

	// Show tower ability information for hovered tower
	for (auto it = context.Iterate<CTags, CTransform, CEntityDebug>(); it.IsValid(); ++it)
	{
		CTags const& tags = *tagsStorage.Get(it);
		bool isTower = tags.HasTag("Tower");
		bool isEnemy = tags.HasTag("enemy");
		bool isProj = tags.HasTag("projectile");
		bool isDebugabble = isTower || isEnemy || isProj;
		if (isDebugabble)
		{
			CTransform const& transform = transStorage[it];
			CCollision const* collision = collisionStorage.Get(it);
			float radius = collision ? collision->m_radius : 2.f;
			if (scDebug.m_debugMouseWorldLocation.GetDistanceSquaredTo(transform.m_pos) < (radius * radius))
			{
				CEntityDebug const& debug = debugStorage[it];
				Vec2 screenPos = worldCamera.m_camera.WorldToScreenRelativeOrtho(transform.m_pos + Vec2(radius, radius)) * screenCamera.GetOrthoDimensions2D();
				Vec2 cardMins = screenPos;

				std::string debugString;

				if (isTower)
				{
					CAbility const* abilityComp = abilityStorage.Get(it);
					if (abilityComp)
					{
						for (auto& ability : abilityComp->m_abilities)
						{
							ability->AppendDebugString(debugString);
						}
					}
				}

				if (isEnemy)
				{
					CHealth const* health = healthStorage.Get(it);
					if (health)
					{
						health->AppendDebugString(debugString);
					}
					CTime const* time = timeStorage.Get(it);
					if (time)
					{
						time->AppendDebugString(debugString);
					}
					CMovement const* movement = movementStorage.Get(it);
					if (movement)
					{
						movement->AppendDebugString(debugString);
					}
				}

				if (isProj)
				{
					CProjectile const* proj = projectileStorage.Get(it);
					if (proj)
					{
						proj->AppendDebugString(debugString);
					}
				}

				Vec2 cardDims = Vec2(300.f, 300.f);
				int numLines = StringUtils::CountStringsByDelimiter(debugString, '\n');
				cardDims.y = 60.f + 30.f * (float) numLines;
				AABB2 informationCardBounds = AABB2(cardMins, cardMins + cardDims);

				// Background
				VertexUtils::AddVertsForAABB2(untexVerts, informationCardBounds, Rgba8::DarkGray);

				// Background Outline
				VertexUtils::AddVertsForWireBox2D(untexVerts, informationCardBounds, 5.f, Rgba8::Black);

				// Title
				font->AddVertsForAlignedText2D(fontVerts, informationCardBounds.GetTopLeft() + Vec2(5.f, -5.f), Vec2(1.f, -1.f), 33.f, debug.m_defName.ToString(), Rgba8::White);

				// Debug Information
				font->AddVertsForAlignedText2D(fontVerts, informationCardBounds.GetTopLeft() + Vec2(5.f, -50.f), Vec2(1.f, -1.f), 25.f, debugString, Rgba8::White, 0.33f);
				break;
			}
		}
	}

	renderer.BeginCamera(&screenCamera);

	// Render->clear debug verts 
	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(untexVerts);
	untexVerts.ClearVerts();

	// Render->clear debug text verts 
	font->SetRendererState(*g_renderer);
	renderer.DrawVertexBuffer(fontVerts);
	fontVerts.ClearVerts();
}



//----------------------------------------------------------------------------------------------------------------------
bool SDebugOverlay::ToggleDebugOverlay(NamedProperties&)
{
	SCDebug& scDebug = g_ecs->GetSingleton<SCDebug>();
	scDebug.m_debugOverlayEnabled = !scDebug.m_debugOverlayEnabled;
	return false;
}
