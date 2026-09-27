// Bradley Christensen - 2022-2026
#include "SRenderSwirls.h"
#include "CAnimation.h"
#include "CRender.h"
#include "CTags.h"
#include "FlavorDef.h"
#include "GameCommon.h"
#include "SCAssetManager.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "SCWorld.h"
#include "SpriteShaderCPU.h"
#include "SwirlShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Renderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderSwirls::Startup()
{
    AddReadDependencies<CAnimation, CRender, CTags, SCCamera>();
    AddWriteDependencies<SCAssetManager, SCRenderer>();

	SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
	AssetManager& assetManager = *scAssetManager.GetAssetManager();

    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    // Flavor constants
	scRenderer.m_flavorConstantsBuffer = renderer.MakeConstantBuffer(sizeof(FlavorConstants));
	ConstantBuffer* flavorCbo = renderer.GetConstantBuffer(scRenderer.m_flavorConstantsBuffer);

	FlavorConstants flavorConstants;
	Vec4 magentaRgb = Vec4(1.f, 0.f, 1.f, 1.f); // default to magenta for missing flavors
    flavorConstants.m_flavorTints.fill(magentaRgb);

	auto const& flavorDefs = FlavorDef::GetAllFlavorDefs(); // Ensure flavor defs are loaded before we try to use them
	for (int i = 0; i < flavorDefs.size(); ++i)
	{
		FlavorDef const& flavorDef = flavorDefs[i];
		flavorDef.m_tint.GetAsFloats(&flavorConstants.m_flavorTints[i].x);
	}
	flavorCbo->Update(flavorConstants);

    // Swirl Shader
    scRenderer.m_swirlShaderAsset = assetManager.AsyncLoad<ShaderAsset>("Data/Shaders/SwirlShader.xml");

    // Swirl Instances
	scRenderer.m_swirlInstanceBuffer = renderer.MakeInstanceBuffer<SwirlInstance>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderSwirls::Shutdown() const
{
    SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
    AssetManager& assetManager = *scAssetManager.GetAssetManager();

    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    // Flavor constants
    renderer.ReleaseConstantBuffer(scRenderer.m_flavorConstantsBuffer);

    // Swirl Shader
    assetManager.Release(scRenderer.m_swirlShaderAsset);

    // Swirl Instances
    renderer.ReleaseInstanceBuffer(scRenderer.m_swirlInstanceBuffer);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderSwirls::Run(SystemContext const& context) const
{
    // Read Dependencies
    auto& animStorage = context.GetArrayStorageConst<CAnimation>();
    auto& tagsStorage = context.GetArrayStorageConst<CTags>();
    auto& renderStorage = context.GetArrayStorageConst<CRender>();
    SCCamera const& scCamera = context.GetSingletonConst<SCCamera>();

	// Write Dependencies
    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    SCAssetManager& scAssetManager = context.GetSingleton<SCAssetManager>();
    AssetManager& assetManager = *scAssetManager.GetAssetManager();

    ShaderAsset const* swirlShaderAsset = assetManager.Get<ShaderAsset>(scRenderer.m_swirlShaderAsset);
    if (swirlShaderAsset == nullptr)
    {
        return;
    }

    ShaderID swirlShaderID = swirlShaderAsset->GetShaderID();
    AABB2 cameraBounds = scCamera.m_worldCamera.GetTranslatedOrthoBounds2D();

    GridSpriteSheet const* iceCreamSpriteSheet = nullptr;

    // Push back an instance for every entity in camera view this frame
    for (auto it = context.Iterate<CRender, CAnimation>(); it.IsValid(); ++it)
    {
        CRender const& render = renderStorage[it];
        if (!render.GetIsInCameraView())
        {
            continue;
        }

        CAnimation const& anim = animStorage[it];
        if (anim.m_renderStyle != SpriteRenderStyle::Swirl || anim.m_gridSpriteSheet == AssetID::Invalid || !anim.m_animInstance.IsValid())
        {
            continue;
        }

		CTags const& tags = tagsStorage[it];

        GridSpriteSheet const* spriteSheet = assetManager.Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
            // not loaded yet
            continue;
        }

		iceCreamSpriteSheet = spriteSheet; // todo: remove this when we have a swirl shader that can handle multiple sprite sheets

        // Get or create instance buffer for this sprite sheet
        InstanceBufferID iboID = scRenderer.m_swirlInstanceBuffer;
        InstanceBuffer* ibo = renderer.GetInstanceBuffer(iboID);

        ASSERT_OR_DIE(ibo != nullptr, "SRenderSwirls::Run - Invalid instance buffer.");

		float renderDepth = render.m_depthOverride;
        if (renderDepth == RenderConstants::s_invalidSpriteRenderDepth)
        {
            // If not overriden, do depth based on y location on the screen, so things get farther back the higher up on the screen they are rendered.
            float baseY = render.GetRenderPosition().y - (render.m_renderRadius);
			float minExpectedSpriteBaseY = cameraBounds.mins.y - RenderConstants::s_maxExpectedSpriteHeight;
            renderDepth = MathUtils::RangeMap(baseY, minExpectedSpriteBaseY, cameraBounds.maxs.y, RenderConstants::s_minSpriteRenderDepth, RenderConstants::s_maxSpriteRenderDepth);
		}

        SwirlInstance instance;
        instance.m_position = Vec3(render.GetRenderPosition(), renderDepth);
        instance.m_orientation = render.GetRenderOrientation();
        instance.m_rgba = render.m_tint;
		instance.m_outlineRgba = render.m_outlineTint;
        instance.m_dims = spriteSheet->GetSpriteDimensions() * render.m_renderRadius * 2.f;
		instance.m_indoorLight = 255; // todo:
		instance.m_outdoorLight = 255; // todo:
        instance.m_spriteIndex = anim.m_animInstance.GetCurrentSpriteIndex();

        instance.m_numFlavors = static_cast<uint8_t>(tags.GetNumFlavorTags());
        if (instance.m_numFlavors == 2)
        {
            instance.m_numSwirls = 3;
            instance.m_swirlRotation = 360.f;
        }
        else if (instance.m_numFlavors == 3)
        {
			instance.m_numSwirls = 2;
            instance.m_swirlRotation = 180.f;
        }

		auto const& flavorDefs = tags.GetFlavorDefs();
		for (int i = 0; i < StaticGameSettings::s_maxFlavorsInOneTower; ++i)
		{
            if (flavorDefs[i])
            {
                instance.m_flavorIndices[i] = static_cast<uint8_t>(flavorDefs[i]->m_flavorIndex);
            }
		}

        ibo->AddInstance(instance);
    }

    if (!iceCreamSpriteSheet)
    {
        // May be reloading right now
        return;
    }

    InstanceBuffer* ibo = renderer.GetInstanceBuffer(scRenderer.m_swirlInstanceBuffer);
    if (!ibo || ibo->GetNumInstances() == 0)
    {
        return;
	}

    ConstantBuffer* spriteCbo = renderer.GetConstantBuffer(scRenderer.m_spriteSheetConstantsBuffer);

    ASSERT_OR_DIE(spriteCbo != nullptr, "SRenderEntities::Run - Invalid constant buffer.");

    SpriteSheetConstants spriteSheetConstants;
    spriteSheetConstants.m_layout = iceCreamSpriteSheet->GetLayout();
    spriteSheetConstants.m_edgePadding = iceCreamSpriteSheet->GetEdgePadding();
    spriteSheetConstants.m_innerPadding = iceCreamSpriteSheet->GetInnerPadding();
    spriteSheetConstants.m_textureDims = iceCreamSpriteSheet->GetTextureDimensions();
    spriteCbo->Update(spriteSheetConstants);

    renderer.BindConstantBuffer(scRenderer.m_spriteSheetConstantsBuffer, SpriteSheetConstants::GetSlot());
    renderer.BindConstantBuffer(scRenderer.m_flavorConstantsBuffer, FlavorConstants::GetSlot());
    iceCreamSpriteSheet->SetRendererState();
    renderer.BindShader(swirlShaderID);
    renderer.DrawInstanced(6, *ibo);

    // Clear out instances we rendererd
    ibo->ClearInstances();
}