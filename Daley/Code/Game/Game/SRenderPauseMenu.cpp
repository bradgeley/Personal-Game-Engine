// Bradley Christensen - 2022-2026
#include "SRenderPauseMenu.h"
#include "Game.h"
#include "SCGame.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "Engine/Assets/Font.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderPauseMenu::Startup()
{
	AddReadDependencies<SCCamera, SCGame>();
	AddWriteDependencies<SCRenderer>();

	SCCamera const& scCamera = g_ecs->GetSingleton<SCCamera>();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	scRenderer.m_pauseMenuBackgroundVBO = renderer.MakeVertexBuffer<Vertex_PCU>();

	VertexBuffer& untexturedVerts = *renderer.GetVertexBuffer(scRenderer.m_pauseMenuBackgroundVBO);

	AABB2 cameraBounds = scCamera.m_camera.GetOrthoBounds2D();

	VertexUtils::AddVertsForAABB2(untexturedVerts, cameraBounds, Rgba8(0, 0, 0, 27));

	Font const* font = renderer.GetDefaultFont();
	if (font)
	{
		scRenderer.m_pauseMenuTextVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
		VertexBuffer& textVerts = *renderer.GetVertexBuffer(scRenderer.m_pauseMenuTextVBO);
		std::string pauseText = "Paused";
		font->AddVertsForAlignedText2D(textVerts, cameraBounds.GetCenter(), Vec2::ZeroVector, 50.f, pauseText, Rgba8(255, 255, 255, 255));
	}
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
	SCCamera const& scCamera = g_ecs->GetSingleton<SCCamera>();
	SCGame const& scGame = context.GetSingletonConst<SCGame>();
	
	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	if (scRenderer.m_pauseMenuTextVBO == RendererUtils::InvalidID)
	{
		Font const* font = renderer.GetDefaultFont();
		if (font != nullptr)
		{
			scRenderer.m_pauseMenuTextVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
			AABB2 cameraBounds = renderer.GetCurrentCamera()->GetOrthoBounds2D();
			VertexBuffer& textVerts = *renderer.GetVertexBuffer(scRenderer.m_pauseMenuTextVBO);
			std::string pauseText = "Paused";
			font->AddVertsForAlignedText2D(textVerts, cameraBounds.GetCenter(), Vec2::ZeroVector, 3.f, pauseText, Rgba8(255, 255, 255, 255));
		}
	}

	if (scGame.m_game->IsPaused())
	{
		renderer.BindTexture();
		renderer.BindShader();
		renderer.DrawVertexBuffer(scRenderer.m_pauseMenuBackgroundVBO);

		Font const* font = renderer.GetDefaultFont();
		font->SetRendererState(renderer);
		renderer.DrawVertexBuffer(scRenderer.m_pauseMenuTextVBO);
	}

}