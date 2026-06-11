// Bradley Christensen - 2022-2026
#include "SRenderUI.h"
#include "CHealth.h"
#include "CRender.h"
#include "CTime.h"
#include "HealthBarShaderCPU.h"
#include "SCRender.h"
#include "SpriteShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Startup()
{
	AddReadDependencies<CHealth, CRender, CTime, SCRender>();
	AddWriteDependencies<Renderer>();

	SCRender& scRender = g_ecs->GetSingleton<SCRender>();

	//----------------------------------------------------------------------------------------------------------------------
	// Health Bar Rendering 

	scRender.m_healthBarConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(HealthBarRenderConstants));
	scRender.m_healthBarInstanceBuffer = g_renderer->MakeInstanceBuffer();

	InstanceBuffer& healthBarIBO = *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	healthBarIBO.Initialize<HealthBarInstance>();

	HealthBarRenderConstants constants;
	Rgba8::DarkGray.GetAsFloats(constants.m_backgroundTint);
	Rgba8::Crimson.GetAsFloats(constants.m_healthTint);
	Rgba8::Green.GetAsFloats(constants.m_poisonTint);
	Rgba8::Orange.GetAsFloats(constants.m_fireTint);

	ConstantBuffer& healthBarCBO = *g_renderer->GetConstantBuffer(scRender.m_healthBarConstantsBuffer);
	healthBarCBO.Update(constants);

	scRender.m_healthBarShaderAsset = g_assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/HealthBarShader.xml");

	//----------------------------------------------------------------------------------------------------------------------
	// Icon Rendering

	scRender.m_iconsVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	scRender.m_iconsInstanceBuffer = g_renderer->MakeInstanceBuffer();

	InstanceBuffer& iconsIBO = *g_renderer->GetInstanceBuffer(scRender.m_iconsInstanceBuffer);
	iconsIBO.Initialize<SpriteInstance>();

	scRender.m_iconsSpriteSheet = g_assetManager->LoadSynchronous<GridSpriteSheet>("Data/SpriteSheets/Icons.xml");
	GridSpriteSheet& iconsSpriteSheet = *g_assetManager->Get<GridSpriteSheet>(scRender.m_iconsSpriteSheet);

	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(scRender.m_iconsVBO);
	VertexUtils::AddVertsForAABB2(vbo, iconsSpriteSheet.GetGenericSpriteQuad(1.f), Rgba8::White);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Run(SystemContext const& context)
{
	// Read Dependencies
	auto const& renderStorage = g_ecs->GetArrayStorage<CRender>();
	auto const& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	auto const& timeStorage = g_ecs->GetArrayStorage<CTime>();
	SCRender const& scRender = g_ecs->GetSingleton<SCRender>();

	// Shaders
	ShaderAsset* spriteShaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_spriteShaderAsset);
	ShaderAsset* healthBarShaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_healthBarShaderAsset);

	// Render Health bars
	InstanceBuffer& healthBarIBO = *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	healthBarIBO.ClearInstances();

	for (auto it = g_ecs->Iterate<CRender, CHealth>(context); it.IsValid(); ++it)
	{
		CRender const& render = renderStorage[it];
		if (!render.GetIsInCameraView())
		{
			continue;
		}

		CHealth const& health = healthStorage[it];
		bool shouldShowHealthBar = !health.GetNeverShowHealthBar() && (health.m_currentHealth < health.m_maxHealth || health.GetBurnSaturation() > 0.f || health.GetPoisonSaturation() > 0.f);

		if (shouldShowHealthBar)
		{
			HealthBarInstance instance;
			instance.m_position = render.GetRenderPosition() + Vec2(0.f, 0.55f * render.m_renderRadius);
			instance.m_dimensions.x = 0.75f * render.m_renderRadius;
			instance.m_dimensions.y = 0.0833f;
			instance.m_fireFraction = health.GetBurnSaturation();
			instance.m_poisonFraction = health.GetPoisonSaturation();
			instance.m_healthFraction = health.GetHealthFraction();
			healthBarIBO.AddInstance(instance);
		}
	}

	// Render Slow Icons
	InstanceBuffer& iconsIBO = *g_renderer->GetInstanceBuffer(scRender.m_iconsInstanceBuffer);
	iconsIBO.ClearInstances();

	GridSpriteSheet const& iconsSpriteSheet = *g_assetManager->Get<GridSpriteSheet>(scRender.m_iconsSpriteSheet);
	SpriteAnimationDef const* slowAnimDef = iconsSpriteSheet.GetAnimationDef("Slow");
	if (!slowAnimDef)
	{
		ERROR_AND_DIE("SRenderUI::Run - Failed to find Slow animation in Icons sprite sheet.");
	}
	SpriteAnimation slowAnim = slowAnimDef->MakeAnimInstance();

	for (auto it = g_ecs->Iterate<CRender, CTime>(context); it.IsValid(); ++it)
	{
		CTime const& time = timeStorage[it];
		if (!time.IsSlowed())
		{
			continue;
		}
		CRender const& render = renderStorage[it];
		if (!render.GetIsInCameraView())
		{
			continue;
		}

		SpriteInstance instance;
		instance.m_position = Vec3(render.GetRenderPosition() + Vec2(0.f, 0.55f * render.m_renderRadius) + Vec2(-0.75f, -0.0f), 0.f);
		instance.m_orientation = 0.f;
		instance.m_scale = 0.2f;
		instance.m_rgba = Rgba8(80, 80, 80, 255);
		instance.m_spriteIndex = slowAnim.GetCurrentFrameIndex();
		instance.m_outdoorLight = 255;
		instance.m_indoorLight = 255;
		iconsIBO.AddInstance(instance);
	}

	if (spriteShaderAsset)
	{
		VertexBuffer& iconsVBO = *g_renderer->GetVertexBuffer(scRender.m_iconsVBO);
		ConstantBuffer* spriteCbo = g_renderer->GetConstantBuffer(scRender.m_spriteSheetConstantsBuffer);

		ASSERT_OR_DIE(spriteCbo != nullptr, "SRenderUI::Run - Invalid constant buffer.");

		SpriteSheetConstants spriteSheetConstants;
		spriteSheetConstants.m_layout = iconsSpriteSheet.GetLayout();
		spriteSheetConstants.m_edgePadding = iconsSpriteSheet.GetEdgePadding();
		spriteSheetConstants.m_innerPadding = iconsSpriteSheet.GetInnerPadding();
		spriteSheetConstants.m_textureDims = iconsSpriteSheet.GetTextureDimensions();
		spriteCbo->Update(spriteSheetConstants);

		iconsSpriteSheet.SetRendererState();
		g_renderer->SetModelMatrix(Mat44::Identity);
		g_renderer->BindShader(spriteShaderAsset->GetShaderID());
		g_renderer->DrawInstanced(iconsVBO, iconsIBO);
	}

	if (healthBarShaderAsset)
	{
		g_renderer->BindConstantBuffer(scRender.m_healthBarConstantsBuffer, 5);
		g_renderer->BindShader(healthBarShaderAsset->GetShaderID());
		g_renderer->DrawInstanced(6, *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Shutdown()
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	g_renderer->ReleaseVertexBuffer(scRender.m_iconsVBO);
	g_renderer->ReleaseInstanceBuffer(scRender.m_iconsInstanceBuffer);
	g_renderer->ReleaseInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	g_renderer->ReleaseConstantBuffer(scRender.m_healthBarConstantsBuffer);
	g_assetManager->Release(scRender.m_healthBarShaderAsset);
	g_assetManager->Release(scRender.m_iconsSpriteSheet);
}