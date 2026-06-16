// Bradley Christensen - 2022-2026
#include "SRenderEntities.h"
#include "CAnimation.h"
#include "CRender.h"
#include "SCCamera.h"
#include "SCRender.h"
#include "SCWorld.h"
#include "SpriteShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/ConstantBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Startup()
{
    AddReadDependencies<CAnimation, CRender, SCCamera>();
    AddWriteDependencies<SCRender, AssetManager, Renderer>();

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();

    scRender.m_spriteSheetConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(SpriteSheetConstants));

    // Sprite Shader
    scRender.m_spriteShaderAsset = g_assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/SpriteShader.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown() const
{
    ASSERT_OR_DIE(g_assetManager != nullptr, "SRenderEntities::Shutdown - Asset manager is null.");
    ASSERT_OR_DIE(g_renderer != nullptr, "SRenderEntities::Shutdown - Renderer is null.");

    SCRender scRender = g_ecs->GetSingleton<SCRender>();

    for (auto it : scRender.m_instancesPerSpriteSheet)
    {
        g_assetManager->Release(it.first);
        g_renderer->ReleaseInstanceBuffer(it.second);
    }

    g_renderer->ReleaseConstantBuffer(scRender.m_spriteSheetConstantsBuffer);
    g_assetManager->Release(scRender.m_spriteShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context) const
{
    // Read Dependencies
    auto& animStorage = context.GetArrayStorageConst<CAnimation>();
    auto& renderStorage = context.GetArrayStorageConst<CRender>();
    SCCamera const& scCamera = context.GetSingletonConst<SCCamera>();

	// Write Dependencies
    SCRender& scRender = context.GetSingleton<SCRender>();
    // g_assetManager
    // g_renderer

    ShaderAsset const* spriteShaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_spriteShaderAsset);
    if (spriteShaderAsset == nullptr)
    {
        return;
    }

    ShaderID spriteShaderID = spriteShaderAsset->GetShaderID();
    AABB2 cameraBounds = scCamera.m_camera.GetTranslatedOrthoBounds2D();

    // Clear last frame's instances
    for (auto it : scRender.m_instancesPerSpriteSheet)
    {
        InstanceBufferID iboID = it.second;
        InstanceBuffer* ibo = g_renderer->GetInstanceBuffer(iboID);
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

        GridSpriteSheet const* spriteSheet = g_assetManager->Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
            // not loaded yet
            continue;
        }

        // Get or create instance buffer for this sprite sheet
        if (scRender.m_instancesPerSpriteSheet.find(anim.m_gridSpriteSheet) == scRender.m_instancesPerSpriteSheet.end())
        {
            scRender.m_instancesPerSpriteSheet[anim.m_gridSpriteSheet] = g_renderer->MakeInstanceBuffer<SpriteInstance>();
        }
        InstanceBufferID iboID = scRender.m_instancesPerSpriteSheet[anim.m_gridSpriteSheet];
        InstanceBuffer* ibo = g_renderer->GetInstanceBuffer(iboID);

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
        instance.m_dims = spriteSheet->GetSpriteDimensions(render.m_renderRadius * 2.f);
		instance.m_indoorLight = 255; // todo:
		instance.m_outdoorLight = 255; // todo:
        instance.m_spriteIndex = anim.m_animInstance.GetCurrentSpriteIndex();

        ibo->AddInstance(instance);
    }

    // 1 Draw call per sprite sheet
    for (auto pair : scRender.m_instancesPerSpriteSheet)
    {
        AssetID assetID = pair.first;
        InstanceBufferID iboID = pair.second;

        GridSpriteSheet const* spriteSheet = g_assetManager->Get<GridSpriteSheet>(assetID);
        if (!spriteSheet)
        {
            // May be reloading right now
            continue;
        }
        InstanceBuffer* ibo = g_renderer->GetInstanceBuffer(iboID);
        if (!ibo || ibo->GetNumInstances() == 0)
        {
            continue;
		}

        ConstantBuffer* spriteCbo = g_renderer->GetConstantBuffer(scRender.m_spriteSheetConstantsBuffer);

        ASSERT_OR_DIE(spriteCbo != nullptr, "SRenderEntities::Run - Invalid constant buffer.");

        SpriteSheetConstants spriteSheetConstants;
        spriteSheetConstants.m_layout = spriteSheet->GetLayout();
        spriteSheetConstants.m_edgePadding = spriteSheet->GetEdgePadding();
        spriteSheetConstants.m_innerPadding = spriteSheet->GetInnerPadding();
        spriteSheetConstants.m_textureDims = spriteSheet->GetTextureDimensions();
        spriteCbo->Update(spriteSheetConstants);

        g_renderer->SetModelConstants(ModelConstants());
        g_renderer->BindConstantBuffer(scRender.m_spriteSheetConstantsBuffer, 5);
        spriteSheet->SetRendererState();
        g_renderer->BindShader(spriteShaderID);
        g_renderer->DrawInstanced(6, *ibo);
    }
}