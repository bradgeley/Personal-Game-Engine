// Bradley Christensen - 2022-2026
#include "SRenderModSelection.h"
#include "EntityDef.h"
#include "SCAssetManager.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "SCRunData.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Font.h"
#include "Engine/Assets/GridSpriteSheet.h"
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
	AddWriteDependencies<SCAssetManager, SCRenderer>();

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

	SCAssetManager& scAssetManager = context.GetSingleton<SCAssetManager>();
	AssetManager& assetManager = *scAssetManager.GetAssetManager();

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

	VertexBufferID spriteVBO = renderer.MakeVertexBuffer<Vertex_PCU>();
	VertexBuffer& spriteVboRef = *renderer.GetVertexBuffer(spriteVBO);
	
	//------------------------------------------------------
	// Render Options
	//
	constexpr float cardScreenHeightPercentage = 0.7f;
	constexpr float cardAspect = 0.667f;
	constexpr float cardSpacing = 20.f;
	float cardHeight = cameraBounds.GetHeight() * cardScreenHeightPercentage;
	Vec2 cardDims = Vec2(cardHeight * cardAspect, cardHeight);
	int numCards = runData.m_numActiveModifierChoices;

	renderer.BeginCamera(&camera.m_uiCamera);

	for (int cardIndex = 0; cardIndex < numCards; ++cardIndex)
	{
		RunModifierDef const* modifierDef = runData.m_modifierChoices[cardIndex];
		if (!modifierDef)
		{
			continue;
		}

		RunModifier const* activeMod = nullptr;
		for (auto& mod : runData.m_activeRunModifiers)
		{
			if (mod->m_def.m_name == modifierDef->m_name)
			{
				modifierDef = &mod->m_def;
				activeMod = mod;
				break;
			}
		}

		std::string cardTitle = modifierDef->m_displayData.m_displayName.ToString();
		std::string cardText = "";
		cardText += "\n";
		if (activeMod)
		{
			activeMod->GetDescription(cardText);
		}
		else
		{
			modifierDef->GetDescription(cardText);
		}

		Vec2 cardTextDims = font->GetTextDims(30.f, 5.f, cardText.c_str());
		Vec2 cardPos = Vec2(cameraBounds.GetCenter().x - (numCards * (cardDims.x + cardSpacing)) / 2.f + (cardIndex * (cardDims.x + cardSpacing)), cameraBounds.GetCenter().y);
		AABB2 cardBounds = AABB2(cardPos, cardPos + cardDims);
		cardBounds.Translate(Vec2(0.f, -cardDims.y / 2.f));

		VertexUtils::AddVertsForAABB2(untexturedVerts, cardBounds, Rgba8(0, 0, 0, 128));

		AssetID spriteSheetID = assetManager.LoadSynchronous<GridSpriteSheet>(modifierDef->m_displayData.m_spriteSheet);
		GridSpriteSheet const* spriteSheet = assetManager.Get<GridSpriteSheet>(spriteSheetID);
		ASSERT_OR_DIE(spriteSheet != nullptr, "SRenderModSelection::Run - Failed to load sprite sheet for run modifier: " + modifierDef->m_displayData.m_spriteSheet.ToString());

		SpriteAnimationDef const* animDef = spriteSheet->GetAnimationDef(modifierDef->m_displayData.m_anim);
		ASSERT_OR_DIE(animDef != nullptr, "SRenderModSelection::Run - Failed to find animation in sprite sheet for run modifier: " + modifierDef->m_displayData.m_anim.ToString());
		SpriteAnimation anim = animDef->MakeAnimInstance();

		AABB2 spriteUVs = spriteSheet->GetSpriteUVs(anim.GetCurrentSpriteIndex());
		float spriteAspect = spriteSheet->GetSpriteAspect();
		Vec2 spriteDims = 0.4f * Vec2(cardDims.x, cardDims.x / spriteAspect);
		AABB2 spriteBounds = AABB2(cardBounds.GetCenter() - spriteDims / 2.f, cardBounds.GetCenter() + spriteDims / 2.f);
		VertexUtils::AddVertsForAABB2(spriteVboRef, spriteBounds, modifierDef->m_displayData.m_tint, spriteUVs);

		std::string buttonText = StringUtils::StringF("Press %d to select", cardIndex + 1);
		float currentTime = context.GetRealTimeSeconds();
		float alpha = 0.5f * (1.f + MathUtils::SinDegrees(360.f * currentTime));
		Rgba8 buttonColor = Rgba8(255, 255, 255, (unsigned char) (alpha * 255.f));

		font->AddVertsForAlignedText2D(textVerts, spriteBounds.GetTopCenter() + Vec2(0.f, 10.f), Vec2(0.f, 1.f), 30.f, cardTitle.c_str(), Rgba8::White, 0.5f);
		font->AddVertsForAlignedText2D(textVerts, spriteBounds.GetBottomCenter() + Vec2(0.f, -10.f), Vec2(0.f, -1.f), 30.f, cardText.c_str(), Rgba8::White, 0.5f);
		font->AddVertsForAlignedText2D(textVerts, cardBounds.GetBottomCenter() + Vec2(0.f, 10.f), Vec2(0.f, 1.f), 30.f, buttonText.c_str(), buttonColor, 0.5f);

		renderer.BindTexture();
		renderer.BindShader();
		renderer.DrawVertexBuffer(scRenderer.m_hudUntexturedVBO);

		spriteSheet->SetRendererState();
		renderer.DrawVertexBuffer(spriteVBO);

		font->SetRendererState(renderer);
		renderer.DrawVertexBuffer(scRenderer.m_hudTextVBO);

		textVerts.ClearVerts();
		untexturedVerts.ClearVerts();
		spriteVboRef.ClearVerts();
	}
}