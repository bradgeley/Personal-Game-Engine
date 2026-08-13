// Bradley Christensen - 2022-2026
#include "SRenderHUD.h"
#include "EntityDef.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "SCRunData.h"
#include "SCWaves.h"
#include "Engine/Assets/Font.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderHUD::Startup()
{
	AddReadDependencies<SCCamera, SCRunData, SCWaves>();
	AddWriteDependencies<SCRenderer>();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	scRenderer.m_hudUntexturedVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
	scRenderer.m_hudTextVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHUD::Shutdown() const
{
	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	if (scRenderer.m_hudUntexturedVBO != RendererUtils::InvalidID)
	{
		renderer.ReleaseVertexBuffer(scRenderer.m_hudUntexturedVBO);
	}
	if (scRenderer.m_hudTextVBO != RendererUtils::InvalidID)
	{
		renderer.ReleaseVertexBuffer(scRenderer.m_hudTextVBO);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHUD::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCRunData const& runData = context.GetSingletonConst<SCRunData>();
	SCCamera const& camera = context.GetSingletonConst<SCCamera>();
	SCWaves const& waves = context.GetSingletonConst<SCWaves>();
	
	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();
	VertexBuffer& untexturedVerts = *renderer.GetVertexBuffer(scRenderer.m_hudUntexturedVBO);
	untexturedVerts.ClearVerts();
	VertexBuffer& textVerts = *renderer.GetVertexBuffer(scRenderer.m_hudTextVBO);
	textVerts.ClearVerts();

	Font const* font = renderer.GetDefaultFont();
	if (!font)
	{
		return;
	}

	AABB2 cameraBounds = camera.m_uiCamera.GetOrthoBounds2D();
	
	//------------------------------------------------------
	// Bottom left corner HUD
	constexpr float hudTextSize = 30.f; // TOOD: make configurable
	constexpr float hudPadding = 20.f; // TOOD: make configurable
	constexpr float hudLineSpacing = 0.25f; // TOOD: make configurable
	std::string hudText = StringUtils::StringF("Gold: %.2f\nHealth: %.2f/%.2f (+%.2f/s)\nInterest Timer (%.2f)\nWave %i/%i (%.2f)", runData.m_gold, runData.m_health, runData.m_maxHealth, runData.m_healthRegen, runData.m_interestTimerSecondsRemaining, waves.m_currentWaveIndex, waves.m_waves.size(), waves.m_waveTimer.GetRemainingSeconds());
	Vec2 hudTextDims = font->GetTextDims(hudTextSize, hudLineSpacing, hudText);
	hudTextDims += Vec2(hudPadding * 2.f, hudPadding * 2.f);

	AABB2 hudBounds = AABB2(Vec2(0.f, 0.f), hudTextDims);
	VertexUtils::AddVertsForAABB2(untexturedVerts, hudBounds, Rgba8(0, 0, 0, 128));
	font->AddVertsForAlignedText2D(textVerts, Vec2(hudPadding, hudPadding), Vec2(1.f, 1.f), hudTextSize, hudText, Rgba8::White, hudLineSpacing);
	//------------------------------------------------------

	//------------------------------------------------------
	// Top Left corner HUD
	std::string placeableTowerText = "Placeable Towers";
	for (auto& placeTower : runData.m_placeableTowers)
	{
		if (placeTower.m_towerName != Name::Invalid)
		{
			placeableTowerText += StringUtils::StringF("\n(%c) - %s : %.0f gold", placeTower.m_hotkey, placeTower.m_towerName.ToCStr(), placeTower.m_cost);
		}
	}
	Vec2 topLeftHUDTextDims = font->GetTextDims(hudTextSize, hudLineSpacing, placeableTowerText);

	AABB2 topLeftHUDBounds = AABB2(Vec2(0.f, cameraBounds.maxs.y - topLeftHUDTextDims.y), Vec2(topLeftHUDTextDims.x, cameraBounds.maxs.y));
	topLeftHUDBounds.mins.y -= hudPadding * 2.f;
	topLeftHUDBounds.maxs.x += hudPadding * 2.f;
	VertexUtils::AddVertsForAABB2(untexturedVerts, topLeftHUDBounds, Rgba8(0, 0, 0, 128));
	font->AddVertsForAlignedText2D(textVerts, topLeftHUDBounds.GetTopLeft() + Vec2(hudPadding, -hudPadding), Vec2(1.f, -1.f), hudTextSize, placeableTowerText, Rgba8::White, hudLineSpacing);
	//------------------------------------------------------

	renderer.BeginCamera(&camera.m_uiCamera);

	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(scRenderer.m_hudUntexturedVBO);

	font->SetRendererState(renderer);
	renderer.DrawVertexBuffer(scRenderer.m_hudTextVBO);
}