// Bradley Christensen - 2022-2026
#include "SRenderStatusIcons.h"
#include "CRender.h"
#include "CTime.h"
#include "SCAssetManager.h"
#include "SCRender.h"
#include "SCRenderer.h"
#include "SpriteShaderCPU.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderStatusIcons::Startup()
{
	AddReadDependencies<CRender, CTime, SCRender>();
	AddWriteDependencies<SCAssetManager, SCRenderer>();

	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	Renderer* renderer = g_ecs->GetSingleton<SCRenderer>().m_renderer;
	AssetManager* assetManager = g_ecs->GetSingleton<SCAssetManager>().m_assetManager;

	scRender.m_iconsInstanceBuffer = renderer->MakeInstanceBuffer();

	InstanceBuffer& iconsIBO = *renderer->GetInstanceBuffer(scRender.m_iconsInstanceBuffer);
	iconsIBO.Initialize<SpriteInstance>();

	scRender.m_iconsSpriteSheet = assetManager->AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/Icons.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderStatusIcons::Shutdown() const
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	Renderer* renderer = g_ecs->GetSingleton<SCRenderer>().m_renderer;
	AssetManager* assetManager = g_ecs->GetSingleton<SCAssetManager>().m_assetManager;

	renderer->ReleaseInstanceBuffer(scRender.m_iconsInstanceBuffer);

	assetManager->Release(scRender.m_iconsSpriteSheet);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderStatusIcons::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& renderStorage = context.GetArrayStorageConst<CRender>();
	auto& timeStorage = context.GetArrayStorageConst<CTime>();
	SCRender const& scRender = context.GetSingletonConst<SCRender>();

	// Write Dependencies
	Renderer* renderer = context.GetSingleton<SCRenderer>().m_renderer;
	AssetManager* assetManager = context.GetSingleton<SCAssetManager>().m_assetManager;

	// Render Slow Icons
	InstanceBuffer& iconsIBO = *renderer->GetInstanceBuffer(scRender.m_iconsInstanceBuffer);
	iconsIBO.ClearInstances();

	GridSpriteSheet const* iconsSpriteSheet = assetManager->Get<GridSpriteSheet>(scRender.m_iconsSpriteSheet);
	if (!iconsSpriteSheet)
	{
		return; // Not loaded yet
	}

	SpriteAnimationDef const* slowAnimDef = iconsSpriteSheet->GetAnimationDef("Slow");
	ASSERT_OR_DIE(slowAnimDef, "SRenderUI::Run - Failed to find Slow animation in Icons sprite sheet.")

	SpriteAnimation slowAnim = slowAnimDef->MakeAnimInstance();

	for (auto it = context.Iterate<CRender, CTime>(); it.IsValid(); ++it)
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
		instance.m_dims = Vec2(0.2f, 0.2f);
		instance.m_rgba = Rgba8(80, 80, 80, 255);
		instance.m_spriteIndex = slowAnim.GetCurrentFrameIndex();
		instance.m_outdoorLight = 255;
		instance.m_indoorLight = 255;
		iconsIBO.AddInstance(instance);
	}

	ShaderAsset const* spriteShaderAsset = assetManager->Get<ShaderAsset>(scRender.m_spriteShaderAsset);
	if (spriteShaderAsset)
	{
		ConstantBuffer* spriteCbo = renderer->GetConstantBuffer(scRender.m_spriteSheetConstantsBuffer);

		ASSERT_OR_DIE(spriteCbo != nullptr, "SRenderStatusIcons::Run - Invalid constant buffer.");

		SpriteSheetConstants spriteSheetConstants;
		spriteSheetConstants.m_layout = iconsSpriteSheet->GetLayout();
		spriteSheetConstants.m_edgePadding = iconsSpriteSheet->GetEdgePadding();
		spriteSheetConstants.m_innerPadding = iconsSpriteSheet->GetInnerPadding();
		spriteSheetConstants.m_textureDims = iconsSpriteSheet->GetTextureDimensions();
		spriteCbo->Update(spriteSheetConstants);

		renderer->BindConstantBuffer(scRender.m_spriteSheetConstantsBuffer, 5);

		iconsSpriteSheet->SetRendererState();
		renderer->SetModelMatrix(Mat44::Identity);
		renderer->BindShader(spriteShaderAsset->GetShaderID());
		renderer->DrawInstanced(6, iconsIBO);
	}
}