// Bradley Christensen - 2022-2026
#include "SRenderPauseMenu.h"
#include "GameState.h"
#include "SCGameState.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "Engine/Assets/Font.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderPauseMenu::Startup()
{
	AddReadDependencies<SCCamera, SCGameState>();
	AddWriteDependencies<SCRenderer>();

	SCCamera const& scCamera = g_ecs->GetSingleton<SCCamera>();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	scRenderer.m_pauseMenuBackgroundVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
	scRenderer.m_pauseMenuTextVBO = renderer.MakeVertexBuffer<Vertex_PCU>();

	VertexBuffer& untexturedVerts = *renderer.GetVertexBuffer(scRenderer.m_pauseMenuBackgroundVBO);

	AABB2 cameraBounds = scCamera.m_uiCamera.GetOrthoBounds2D();

	VertexUtils::AddVertsForAABB2(untexturedVerts, cameraBounds, Rgba8(0, 0, 0, 27));
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderPauseMenu::Shutdown() const
{
	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	renderer.ReleaseVertexBuffer(scRenderer.m_pauseMenuBackgroundVBO);
	renderer.ReleaseVertexBuffer(scRenderer.m_pauseMenuTextVBO);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderPauseMenu::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCGameState const& scGameState = context.GetSingletonConst<SCGameState>();
	SCCamera const& scCamera = context.GetSingletonConst<SCCamera>();
	
	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	if (scGameState.m_gameState->IsPaused())
	{
		renderer.BeginCamera(&scCamera.m_uiCamera);
		renderer.BindTexture();
		renderer.BindShader();
		renderer.DrawVertexBuffer(scRenderer.m_pauseMenuBackgroundVBO);

		Font const* font = renderer.GetDefaultFont();
		if (font)
		{
			AABB2 cameraBounds = scCamera.m_uiCamera.GetOrthoBounds2D();
			VertexBuffer& textVerts = *renderer.GetVertexBuffer(scRenderer.m_pauseMenuTextVBO);
			textVerts.ClearVerts();
			std::string pauseText = "Paused";
			font->AddVertsForAlignedText2D(textVerts, cameraBounds.GetCenter(), Vec2::ZeroVector, 75.f, pauseText, Rgba8(255, 255, 255, 255));

			font->SetRendererState(renderer);
			renderer.DrawVertexBuffer(scRenderer.m_pauseMenuTextVBO);
		}
	}
}