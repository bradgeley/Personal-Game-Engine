// Bradley Christensen - 2022-2026
#include "SRenderModSelection.h"
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
void SRenderModSelection::Startup()
{
	AddReadDependencies<SCCamera, SCRunData>();
	AddWriteDependencies<SCRenderer>();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	scRenderer.m_hudUntexturedVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
	scRenderer.m_hudTextVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderModSelection::Shutdown() const
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
void SRenderModSelection::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCRunData const& scRunData = context.GetSingletonConst<SCRunData>();
	RunData const& runData = *scRunData.m_data;

	if (runData.m_numActiveModifierChoices == 0)
	{
		return;
	}

	SCCamera const& camera = context.GetSingletonConst<SCCamera>();
	
	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();
	VertexBuffer& untexturedVerts = *renderer.GetVertexBuffer(scRenderer.m_hudUntexturedVBO);
	untexturedVerts.ClearVerts();
	VertexBuffer& textVerts = *renderer.GetVertexBuffer(scRenderer.m_hudTextVBO);
	textVerts.ClearVerts();

	if (!scRenderer.m_isHudEnabled)
	{
		return;
	}

	Font const* font = renderer.GetDefaultFont();
	if (!font)
	{
		return;
	}

	AABB2 cameraBounds = camera.m_uiCamera.GetOrthoBounds2D();
	
	//------------------------------------------------------
	// Render Options
	//
	constexpr float cardScreenHeightPercentage = 0.8f;
	constexpr float cardAspect = 0.667f;
	constexpr float cardSpacing = 20.f;
	float cardHeight = cameraBounds.GetHeight() * cardScreenHeightPercentage;
	Vec2 cardDims = Vec2(cardHeight * cardAspect, cardHeight);
	int numCards = runData.m_numActiveModifierChoices;

	for (int cardIndex = 0; cardIndex < numCards; ++cardIndex)
	{
		RunModifierDef const* modifierDef = runData.m_modifierChoices[cardIndex];
		if (!modifierDef)
		{
			continue;
		}
		std::string cardText = StringUtils::StringF("(%i) - %s", cardIndex + 1, modifierDef->m_name.ToCStr());
		Vec2 cardTextDims = font->GetTextDims(30.f, 5.f, cardText.c_str());
		Vec2 cardPos = Vec2(cameraBounds.GetCenter().x - (numCards * (cardDims.x + cardSpacing)) / 2.f + (cardIndex * (cardDims.x + cardSpacing)), cameraBounds.GetCenter().y);
		AABB2 cardBounds = AABB2(cardPos, cardPos + cardDims);
		cardBounds.Translate(Vec2(0.f, -cardDims.y / 2.f));

		VertexUtils::AddVertsForAABB2(untexturedVerts, cardBounds, Rgba8(0, 0, 0, 128));
		font->AddVertsForAlignedText2D(textVerts, cardBounds.GetCenter(), Vec2::ZeroVector, 30.f, cardText.c_str(), Rgba8::White, 5.f);
	}

	renderer.BeginCamera(&camera.m_uiCamera);

	renderer.BindTexture();
	renderer.BindShader();
	renderer.DrawVertexBuffer(scRenderer.m_hudUntexturedVBO);

	font->SetRendererState(renderer);
	renderer.DrawVertexBuffer(scRenderer.m_hudTextVBO);
}