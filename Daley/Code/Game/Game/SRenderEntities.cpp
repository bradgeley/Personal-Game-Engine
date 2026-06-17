// Bradley Christensen - 2022-2026
#include "SRenderEntities.h"
#include "CAnimation.h"
#include "CRender.h"
#include "SCAssetManager.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "SCWorld.h"
#include "SpriteShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/ConstantBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Startup()
{
    AddReadDependencies<CAnimation, CRender, SCCamera>();
    AddWriteDependencies<SCAssetManager, SCRenderer>();

	SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
	AssetManager& assetManager = *scAssetManager.GetAssetManager();

    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    scRenderer.m_spriteSheetConstantsBuffer = renderer.MakeConstantBuffer(sizeof(SpriteSheetConstants));

    // Sprite Shader
    scRenderer.m_spriteShaderAsset = assetManager.AsyncLoad<ShaderAsset>("Data/Shaders/SpriteShader.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown() const
{
    SCAssetManager& scAssetManager = g_ecs->GetSingleton<SCAssetManager>();
    AssetManager& assetManager = *scAssetManager.GetAssetManager();

    SCRenderer& scRenderer = g_ecs->GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    for (auto it : scRenderer.m_instancesPerSpriteSheet)
    {
        assetManager.Release(it.first);
        renderer.ReleaseInstanceBuffer(it.second);
    }

    renderer.ReleaseConstantBuffer(scRenderer.m_spriteSheetConstantsBuffer);
    assetManager.Release(scRenderer.m_spriteShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context) const
{
    // Read Dependencies
    auto& animStorage = context.GetArrayStorageConst<CAnimation>();
    auto& renderStorage = context.GetArrayStorageConst<CRender>();
    SCCamera const& scCamera = context.GetSingletonConst<SCCamera>();

	// Write Dependencies
    SCRenderer& scRenderer = context.GetSingleton<SCRenderer>();
    Renderer& renderer = *scRenderer.GetRenderer();

    SCAssetManager& scAssetManager = context.GetSingleton<SCAssetManager>();
    AssetManager& assetManager = *scAssetManager.GetAssetManager();

    ShaderAsset const* spriteShaderAsset = assetManager.Get<ShaderAsset>(scRenderer.m_spriteShaderAsset);
    if (spriteShaderAsset == nullptr)
    {
        return;
    }

    ShaderID spriteShaderID = spriteShaderAsset->GetShaderID();
    AABB2 cameraBounds = scCamera.m_camera.GetTranslatedOrthoBounds2D();

    // Clear last frame's instances
    for (auto it : scRenderer.m_instancesPerSpriteSheet)
    {
        InstanceBufferID iboID = it.second;
        InstanceBuffer* ibo = renderer.GetInstanceBuffer(iboID);
        if (ibo)
        {
            ibo->ClearInstances();
        }
    }

    // Push back an instance for every entity in camera view this frame
    for (auto it = context.Iterate<CRender, CAnimation>(); it.IsValid(); ++it)
    {
        CRender const& render = renderStorage[it];
        if (!render.GetIsInCameraView())
        {
            continue;
        }

        CAnimation const& anim = animStorage[it];
        if (anim.m_gridSpriteSheet == AssetID::Invalid || !anim.m_animInstance.IsValid())
        {
            continue;
        }

        GridSpriteSheet const* spriteSheet = assetManager.Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
            // not loaded yet
            continue;
        }

        // Get or create instance buffer for this sprite sheet
        if (scRenderer.m_instancesPerSpriteSheet.find(anim.m_gridSpriteSheet) == scRenderer.m_instancesPerSpriteSheet.end())
        {
            scRenderer.m_instancesPerSpriteSheet[anim.m_gridSpriteSheet] = renderer.MakeInstanceBuffer<SpriteInstance>();
        }
        InstanceBufferID iboID = scRenderer.m_instancesPerSpriteSheet[anim.m_gridSpriteSheet];
        InstanceBuffer* ibo = renderer.GetInstanceBuffer(iboID);

        ASSERT_OR_DIE(ibo != nullptr, "SRenderEntities::Run - Invalid instance buffer.");

		float renderDepth = render.m_depthOverride;
        if (renderDepth == RenderConstants::s_invalidSpriteRenderDepth)
        {
            // If not overriden, do depth based on y location on the screen, so things get farther back the higher up on the screen they are rendered.
            float baseY = render.GetRenderPosition().y - (render.m_renderRadius);
			float minExpectedSpriteBaseY = cameraBounds.mins.y - RenderConstants::s_maxExpectedSpriteHeight;
            renderDepth = MathUtils::RangeMap(baseY, minExpectedSpriteBaseY, cameraBounds.maxs.y, RenderConstants::s_minSpriteRenderDepth, RenderConstants::s_maxSpriteRenderDepth);
		}

        SpriteInstance instance;
        instance.m_position = Vec3(render.GetRenderPosition(), renderDepth);
        instance.m_orientation = render.GetRenderOrientation();
        instance.m_rgba = render.m_tint;
        instance.m_dims = spriteSheet->GetSpriteDimensions() * render.m_renderRadius * 2.f;
		instance.m_indoorLight = 255; // todo:
		instance.m_outdoorLight = 255; // todo:
        instance.m_spriteIndex = anim.m_animInstance.GetCurrentSpriteIndex();

        ibo->AddInstance(instance);
    }

    // 1 Draw call per sprite sheet
    for (auto pair : scRenderer.m_instancesPerSpriteSheet)
    {
        AssetID assetID = pair.first;
        InstanceBufferID iboID = pair.second;

        GridSpriteSheet const* spriteSheet = assetManager.Get<GridSpriteSheet>(assetID);
        if (!spriteSheet)
        {
            // May be reloading right now
            continue;
        }
        InstanceBuffer* ibo = renderer.GetInstanceBuffer(iboID);
        if (!ibo || ibo->GetNumInstances() == 0)
        {
            continue;
		}

        ConstantBuffer* spriteCbo = renderer.GetConstantBuffer(scRenderer.m_spriteSheetConstantsBuffer);

        ASSERT_OR_DIE(spriteCbo != nullptr, "SRenderEntities::Run - Invalid constant buffer.");

        SpriteSheetConstants spriteSheetConstants;
        spriteSheetConstants.m_layout = spriteSheet->GetLayout();
        spriteSheetConstants.m_edgePadding = spriteSheet->GetEdgePadding();
        spriteSheetConstants.m_innerPadding = spriteSheet->GetInnerPadding();
        spriteSheetConstants.m_textureDims = spriteSheet->GetTextureDimensions();
        spriteCbo->Update(spriteSheetConstants);

        renderer.SetModelConstants(ModelConstants());
        renderer.BindConstantBuffer(scRenderer.m_spriteSheetConstantsBuffer, 5);
        spriteSheet->SetRendererState();
        renderer.BindShader(spriteShaderID);
        renderer.DrawInstanced(6, *ibo);
    }
}