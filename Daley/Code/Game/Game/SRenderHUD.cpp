// Bradley Christensen - 2022-2026
#include "SRenderHUD.h"
#include "EntityDef.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "SCRunData.h"
#include "SCWindow.h"
#include "Engine/Assets/Font.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderHUD::Startup()
{
	AddReadDependencies<SCCamera, SCRunData, SCWindow>();
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
	SCWindow const& window = context.GetSingletonConst<SCWindow>();
	SCCamera const& camera = context.GetSingletonConst<SCCamera>();
	
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

	constexpr float hudTextSize = 30.f; // TOOD: make configurable
	constexpr float hudPadding = 20.f; // TOOD: make configurable
	constexpr float hudLineSpacing = 0.25f; // TOOD: make configurable
	std::string hudText = StringUtils::StringF("Gold: %.2f\nHealth: %.2f/%.2f (+%.2f/s)\nInterest Timer (%.2f)", runData.m_gold, runData.m_currentHealth, runData.m_maxHealth, runData.m_healthRegen, runData.m_interestTimerSecondsRemaining);
	Vec2 hudTextDims = font->GetTextDims(hudTextSize, hudLineSpacing, hudText);
	hudTextDims += Vec2(hudPadding * 2.f, hudPadding * 2.f);

	IntVec2 windowResolution = window.GetWindow()->GetActualWindowResolution();

	AABB2 hudBounds = AABB2(Vec2(0.f, 0.f), hudTextDims);
	VertexUtils::AddVertsForAABB2(untexturedVerts, hudBounds, Rgba8(0, 0, 0, 128));

	renderer.BeginCamera(&camera.m_uiCamera);

	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(scRenderer.m_hudUntexturedVBO);

	font->SetRendererState(renderer);
	font->AddVertsForAlignedText2D(textVerts, Vec2(hudPadding, hudPadding), Vec2(1.f, 1.f), hudTextSize, hudText, Rgba8::White, hudLineSpacing);
	renderer.DrawVertexBuffer(scRenderer.m_hudTextVBO);
}