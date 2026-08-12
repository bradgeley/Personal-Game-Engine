// Bradley Christensen - 2022-2026
#include "SRenderPopupOverlay.h"
#include "GameState.h"
#include "SCGameState.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "SCRunData.h"
#include "SCWaves.h"
#include "Engine/Assets/Font.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderPopupOverlay::Startup()
{
	AddReadDependencies<SCCamera, SCGameState, SCRunData, SCWaves>();
	AddWriteDependencies<SCRenderer>();

	SCCamera const& scCamera = g_ecs->GetSingleton<SCCamera>();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	scRenderer.m_popupOverlayBackgroundVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
	scRenderer.m_popupOverlayTextVBO = renderer.MakeVertexBuffer<Vertex_PCU>();

	VertexBuffer& untexturedVerts = *renderer.GetVertexBuffer(scRenderer.m_popupOverlayBackgroundVBO);

	AABB2 cameraBounds = scCamera.m_uiCamera.GetOrthoBounds2D();

	VertexUtils::AddVertsForAABB2(untexturedVerts, cameraBounds, Rgba8(0, 0, 0, 27));
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderPopupOverlay::Shutdown() const
{
	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	renderer.ReleaseVertexBuffer(scRenderer.m_popupOverlayBackgroundVBO);
	renderer.ReleaseVertexBuffer(scRenderer.m_popupOverlayTextVBO);
}



//----------------------------------------------------------------------------------------------------------------------
enum class PopupOverlayType
{
	None,
	PauseMenu,
	Defeat,
	Victory,
};



//----------------------------------------------------------------------------------------------------------------------
void SRenderPopupOverlay::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCGameState const& scGameState = context.GetSingletonConst<SCGameState>();
	SCCamera const& scCamera = context.GetSingletonConst<SCCamera>();
	SCRunData const& scRunData = context.GetSingletonConst<SCRunData>();
	SCWaves const& scWaves = context.GetSingletonConst<SCWaves>();
	
	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	PopupOverlayType overlayType = PopupOverlayType::None;

	if (scRunData.m_currentHealth <= 0.f)
	{
		overlayType = PopupOverlayType::Defeat;
	}
	else if (scWaves.m_wavesFinished)
	{
		overlayType = PopupOverlayType::Victory;
	}
	else if (scGameState.m_gameState->IsPaused())
	{
		overlayType = PopupOverlayType::PauseMenu;
	}

	if (overlayType != PopupOverlayType::None)
	{
		renderer.BeginCamera(&scCamera.m_uiCamera);
		renderer.BindTexture();
		renderer.BindShader();
		renderer.DrawVertexBuffer(scRenderer.m_popupOverlayBackgroundVBO);

		std::string text;
		std::string footerText;
		Rgba8 textColor;
		if (overlayType == PopupOverlayType::PauseMenu)
		{
			text = "PAUSED";
			footerText = "Press space to resume";
			textColor = Rgba8(255, 255, 255, 255);
		}
		else if (overlayType == PopupOverlayType::Defeat)
		{
			text = "DEFEAT";
			footerText = "Press space to return to main menu";
			textColor = Rgba8(255, 0, 0, 255);
		}
		else // if (overlayType == PopupOverlayType::Victory)
		{
			text = "VICTORY";
			footerText = "Press space to continue";
			textColor = Rgba8(0, 255, 0, 255);
		}

		Font const* font = renderer.GetDefaultFont();
		if (font)
		{
			AABB2 cameraBounds = scCamera.m_uiCamera.GetOrthoBounds2D();
			VertexBuffer& textVerts = *renderer.GetVertexBuffer(scRenderer.m_popupOverlayTextVBO);
			textVerts.ClearVerts();
			font->AddVertsForAlignedText2D(textVerts, cameraBounds.GetCenter(), Vec2::ZeroVector, 75.f, text, textColor);
			font->AddVertsForAlignedText2D(textVerts, cameraBounds.GetCenter() - Vec2(0.f, 50.f), Vec2::ZeroVector, 35.f, footerText, textColor);

			font->SetRendererState(renderer);
			renderer.DrawVertexBuffer(scRenderer.m_popupOverlayTextVBO);
		}
	}
}