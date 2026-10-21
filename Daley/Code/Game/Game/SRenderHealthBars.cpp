// Bradley Christensen - 2022-2026
#include "SRenderHealthBars.h"
#include "CHealth.h"
#include "CRender.h"
#include "CTags.h"
#include "CTime.h"
#include "HealthBarShaderCPU.h"
#include "SCAssetManager.h"
#include "SCRenderer.h"
#include "SpriteShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
// This list must match the order and count of the icons in the HealthBarIcons sprite sheet
//
constexpr int s_healthBarIconCount = 17;

enum class HealthBarIconBit : uint32_t
{
	Bulky = 1 << 0,
	Speedy = 1 << 1,
	Armored = 1 << 2,
	ColdBlooded = 1 << 3,
	IronStomach = 1 << 4,

	Protective = 1 << 5,
	Protected = 1 << 6,
	Hasty = 1 << 7,
	Hasted = 1 << 8,
	Slowing = 1 << 9,
	Slowed = 1 << 10,
	Fireman = 1 << 11,
	SuppressingBurn = 1 << 12,
	Doctor = 1 << 13,
	SuppressingPoison = 1 << 14,
	Healer = 1 << 15,
	BeingHealed = 1 << 16
};

static std::array<Name, s_healthBarIconCount> GetHealthBarIconNames()
{
	return {
		"Bulky",
		"Speedy",
		"Armored",
		"ColdBlooded",
		"IronStomach",

		"Protective",
		"Protected",
		"Hasty",
		"Hasted",
		"Slowing",
		"Slowed",
		"Fireman",
		"SuppressingBurn",
		"Doctor",
		"SuppressingPoison",
		"Healer",
		"BeingHealed"
	};
}	



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Startup()
{
	AddReadDependencies<CHealth, CRender, CTags, CTime, SCAssetManager>();
	AddWriteDependencies<SCRenderer>();

	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();
	SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
	AssetManager& assetManager = *scAssetManager.GetAssetManager();

	scRenderer.m_healthBarConstantsBuffer = renderer.MakeConstantBuffer(sizeof(HealthBarRenderConstants));
	scRenderer.m_healthBarInstanceBuffer = renderer.MakeInstanceBuffer();

	InstanceBuffer& healthBarIBO = *renderer.GetInstanceBuffer(scRenderer.m_healthBarInstanceBuffer);
	healthBarIBO.Initialize<HealthBarInstance>();

	HealthBarRenderConstants constants;
	Rgba8::DarkGray.GetAsFloats(constants.m_backgroundTint);
	Rgba8::Crimson.GetAsFloats(constants.m_healthTint);
	Rgba8::Green.GetAsFloats(constants.m_poisonTint);
	Rgba8::Orange.GetAsFloats(constants.m_fireTint);

	ConstantBuffer& healthBarCBO = *renderer.GetConstantBuffer(scRenderer.m_healthBarConstantsBuffer);
	healthBarCBO.Update(constants);

	scRenderer.m_healthBarIconsSpriteSheet = assetManager.AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/HealthBarIcons.xml");
	scRenderer.m_healthBarShaderAsset = assetManager.AsyncLoad<ShaderAsset>("Data/Shaders/HealthBarShader.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Shutdown() const
{
	SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();
	SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
	AssetManager& assetManager = *scAssetManager.GetAssetManager();

	renderer.ReleaseConstantBuffer(scRenderer.m_healthBarConstantsBuffer);
	renderer.ReleaseInstanceBuffer(scRenderer.m_healthBarInstanceBuffer);

	assetManager.Release(scRenderer.m_healthBarIconsSpriteSheet);
	assetManager.Release(scRenderer.m_healthBarShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	auto& renderStorage = context.GetArrayStorageConst<CRender>();
	auto& tagsStorage = context.GetArrayStorageConst<CTags>();
	auto& timeStorage = context.GetArrayStorageConst<CTime>();
	SCAssetManager const& scAssetManager = context.GetSingletonConst<SCAssetManager>();
	AssetManager const& assetManager = *scAssetManager.GetAssetManager();

	// Write Dependencies
	SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
	Renderer& renderer = *scRenderer.GetRenderer();

	ShaderAsset const* healthBarShaderAsset = assetManager.Get<ShaderAsset>(scRenderer.m_healthBarShaderAsset);
	GridSpriteSheet const* healthBarIconsSpriteSheet = assetManager.Get<GridSpriteSheet>(scRenderer.m_healthBarIconsSpriteSheet);
	ConstantBuffer* spriteSheetCbo = renderer.GetConstantBuffer(scRenderer.m_spriteSheetConstantsBuffer);

	if (!healthBarShaderAsset || !healthBarIconsSpriteSheet || !spriteSheetCbo)
	{
		return;
	}

	InstanceBuffer& healthBarIBO = *renderer.GetInstanceBuffer(scRenderer.m_healthBarInstanceBuffer);
	healthBarIBO.ClearInstances();

	auto const& healthBarIconNames = GetHealthBarIconNames();

	int maxNumIconsNeededToDraw = 0;

	for (auto it = context.Iterate<CRender, CHealth, CTags, CTime>(); it.IsValid(); ++it)
	{
		CRender const& render = renderStorage[it];
		if (!render.GetIsInCameraView())
		{
			continue;
		}

		HealthBarInstance instance;
		instance.m_iconMask = 0;

		int numIcons = 0;

		CTags const& tags = tagsStorage[it];
		for (int iconIndex = 0; iconIndex < s_healthBarIconCount; ++iconIndex)
		{
			Name const& iconTag = healthBarIconNames[iconIndex];
			if (tags.HasTag(iconTag))
			{
				instance.m_iconMask |= (1 << iconIndex);
				numIcons++;
			}
		}

		CHealth const& health = healthStorage[it];
		CTime const& time = timeStorage[it];

		if (health.GetIsProtected())
		{
			instance.m_iconMask |= static_cast<uint32_t>(HealthBarIconBit::Protected);
			numIcons++;
		}
		if (time.IsHasted())
		{
			instance.m_iconMask |= static_cast<uint32_t>(HealthBarIconBit::Hasted);
			numIcons++;
		}
		if (time.IsSlowed())
		{
			instance.m_iconMask |= static_cast<uint32_t>(HealthBarIconBit::Slowed);
			numIcons++;
		}
		if (health.GetIsSuppressingBurn())
		{
			instance.m_iconMask |= static_cast<uint32_t>(HealthBarIconBit::SuppressingBurn);
			numIcons++;
		}
		if (health.GetIsSuppressingPoison())
		{
			instance.m_iconMask |= static_cast<uint32_t>(HealthBarIconBit::SuppressingPoison);
			numIcons++;
		}

		bool shouldShowHealthBar = (numIcons != 0 || health.m_currentHealth < health.m_maxHealth || health.GetBurnSaturation() > 0.f || health.GetPoisonSaturation() > 0.f);
		if (!shouldShowHealthBar)
		{
			continue;
		}

		if (numIcons > maxNumIconsNeededToDraw)
		{
			maxNumIconsNeededToDraw = numIcons;
		}

		instance.m_position = render.GetRenderPosition() + Vec2(0.f, 0.55f * render.m_renderRadius);
		instance.m_dimensions.x = 0.75f * render.m_renderRadius * 2.f;
		instance.m_dimensions.y = 0.0833f;
		instance.m_fireFraction = health.GetBurnSaturation();
		instance.m_poisonFraction = health.GetPoisonSaturation();
		instance.m_healthFraction = health.GetHealthFraction();
		healthBarIBO.AddInstance(instance);
	}

	// Bind icons sprite sheet constants
	SpriteSheetConstants spriteSheetConstants;
	spriteSheetConstants.m_layout = healthBarIconsSpriteSheet->GetLayout();
	spriteSheetConstants.m_edgePadding = healthBarIconsSpriteSheet->GetEdgePadding();
	spriteSheetConstants.m_innerPadding = healthBarIconsSpriteSheet->GetInnerPadding();
	spriteSheetConstants.m_textureDims = healthBarIconsSpriteSheet->GetTextureDimensions();
	spriteSheetCbo->Update(spriteSheetConstants);
	renderer.BindConstantBuffer(scRenderer.m_spriteSheetConstantsBuffer, SpriteSheetConstants::GetSlot());

	// Bind icons texture and sampler settings
	healthBarIconsSpriteSheet->SetRendererState();

	// Bind health bar constants and shader
	renderer.BindConstantBuffer(scRenderer.m_healthBarConstantsBuffer, HealthBarRenderConstants::GetSlot());
	renderer.BindShader(healthBarShaderAsset->GetShaderID());

	// Draw 
	// 
	// Need to render 6 verts per icon plus 6 for the health bar itself. The entity with the max number of icons determines 
	// the vert count for all instances, and any extra verts are drawn at 0,0,0 for entities that dont have that many icons.
	int numVertsPerInstance = 6 + 6 * maxNumIconsNeededToDraw;
	renderer.DrawInstanced(numVertsPerInstance, *renderer.GetInstanceBuffer(scRenderer.m_healthBarInstanceBuffer));
}