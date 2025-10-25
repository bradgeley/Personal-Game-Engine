﻿// Bradley Christensen - 2022-2025
#include "SRenderEntities.h"
#include "CAnimation.h"
#include "Chunk.h"
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
    AddWriteDependencies<Renderer, SCRender>();
    AddReadDependencies<CRender, SCWorld, SCCamera, CAnimation>();

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    scRender.m_spriteSheetConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(SpriteSheetConstants));

    // Sprite Shader
    scRender.m_spriteShaderAsset = g_assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/SpriteShader.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context)
{
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& animStorage = g_ecs->GetArrayStorage<CAnimation>();
    SCCamera const& scCamera = g_ecs->GetSingleton<SCCamera>();
    SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();
    SCRender& scRender = g_ecs->GetSingleton<SCRender>();

    ShaderAsset* spriteShaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_spriteShaderAsset);
    if (spriteShaderAsset)
    {
        scRender.m_spriteShaderID = spriteShaderAsset->GetShaderID();
    }
    else
    {
        scRender.m_spriteShaderID = RendererUtils::InvalidID;
        return;
    }

    AABB2 cameraBounds = scCamera.m_camera.GetTranslatedOrthoBounds2D();

    // Clear last frame's instances
    for (auto it : scRender.m_entityVBOsBySpriteSheet)
    {
        InstanceBufferID iboID = scRender.instancesPerSpriteSheet[it.first];
        InstanceBuffer* ibo = g_renderer->GetInstanceBuffer(iboID);
        if (ibo)
        {
            ibo->ClearInstances();
        }
    }

    // Push back an instance for every entity in camera view this frame
    for (auto renderIt = g_ecs->Iterate<CRender, CAnimation>(context); renderIt.IsValid(); ++renderIt)
    {
        CRender const& render = *renderStorage.Get(renderIt);
        if (!render.GetIsInCameraView())
        {
            continue;
        }

        CAnimation const& anim = *animStorage.Get(renderIt);
        if (anim.m_gridSpriteSheet == AssetID::Invalid || !anim.m_animInstance.IsValid())
        {
            continue;
        }

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(anim.m_gridSpriteSheet);
        if (!spriteSheet)
        {
            // not loaded yet
            continue;
        }

        // Get or create vertex buffer for this sprite sheet
        if (scRender.m_entityVBOsBySpriteSheet.find(anim.m_gridSpriteSheet) == scRender.m_entityVBOsBySpriteSheet.end())
        {
            scRender.m_entityVBOsBySpriteSheet[anim.m_gridSpriteSheet] = g_renderer->MakeVertexBuffer<Vertex_PCU>();
            VertexBufferID vboID = scRender.m_entityVBOsBySpriteSheet[anim.m_gridSpriteSheet];

            VertexBuffer& vbo = *g_renderer->GetVertexBuffer(vboID);
            VertexUtils::AddVertsForAABB2(vbo, spriteSheet->GetGenericSpriteQuad(1.f), render.m_tint); // UVs will be passed deduced via information in the sprite instance data in the shader
        }

        // Get or create instance buffer for this sprite sheet
        if (scRender.instancesPerSpriteSheet.find(anim.m_gridSpriteSheet) == scRender.instancesPerSpriteSheet.end())
        {
            scRender.instancesPerSpriteSheet[anim.m_gridSpriteSheet] = g_renderer->MakeInstanceBuffer<SpriteInstance>();
        }
        InstanceBufferID iboID = scRender.instancesPerSpriteSheet[anim.m_gridSpriteSheet];
        InstanceBuffer& ibo = *g_renderer->GetInstanceBuffer(iboID);

        SpriteInstance instance;
        float instanceDepth = MathUtils::RangeMap(render.GetRenderPosition().y - (render.m_renderRadius * 0.5f), cameraBounds.mins.y - 100.f, cameraBounds.maxs.y + 100.f, 0.05f, 0.95f);
        instance.m_position = Vec3(render.GetRenderPosition(), instanceDepth); // todo: z-order
        instance.m_orientation = render.GetRenderOrientation();
        instance.m_scale = render.m_renderRadius;
        instance.m_rgba = render.m_tint;
        instance.m_rgba = render.m_tint;
        instance.m_spriteIndex = anim.m_animInstance.GetCurrentSpriteIndex();

        WorldCoords worldCoords = scWorld.GetWorldCoordsAtLocation(render.m_pos); // actual pos here, lighting is from the tile the entity is standing on
        Chunk* chunk = scWorld.GetActiveChunk(worldCoords);
        if (chunk)
        {
            Tile& tile = chunk->m_tiles.GetRef(worldCoords.m_localTileCoords);
            instance.m_indoorLight = tile.GetIndoorLighting255();
            instance.m_outdoorLight = tile.GetOutdoorLighting255();
        }

        ibo.AddInstance(instance);
    }

    // 1 Draw call per sprite sheet
    for (auto pair : scRender.m_entityVBOsBySpriteSheet)
    {
        AssetID assetID = pair.first;
        VertexBufferID vboID = pair.second;
        InstanceBufferID iboID = scRender.instancesPerSpriteSheet[assetID];

        GridSpriteSheet* spriteSheet = g_assetManager->Get<GridSpriteSheet>(assetID);
        VertexBuffer* vbo = g_renderer->GetVertexBuffer(vboID);
        InstanceBuffer* ibo = g_renderer->GetInstanceBuffer(iboID);
        ConstantBuffer* spriteCbo = g_renderer->GetConstantBuffer(scRender.m_spriteSheetConstantsBuffer);

        ASSERT_OR_DIE(spriteSheet != nullptr, "SRenderEntities::Run - Invalid sprite sheet asset.");
        ASSERT_OR_DIE(vbo != nullptr, "SRenderEntities::Run - Invalid vertex buffer.");
        ASSERT_OR_DIE(ibo != nullptr, "SRenderEntities::Run - Invalid instance buffer.");
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
        g_renderer->BindShader(scRender.m_spriteShaderID);
        g_renderer->DrawInstanced(*vbo, *ibo);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown()
{
    ASSERT_OR_DIE(g_assetManager != nullptr, "SRenderEntities::Shutdown - Asset manager is null.");
    ASSERT_OR_DIE(g_renderer != nullptr, "SRenderEntities::Shutdown - Renderer is null.");

    SCRender scRender = g_ecs->GetSingleton<SCRender>();
    for (auto it : scRender.m_entityVBOsBySpriteSheet)
    {
        g_assetManager->Release(it.first);
        g_renderer->ReleaseVertexBuffer(it.second);
    }

    g_renderer->ReleaseConstantBuffer(scRender.m_spriteSheetConstantsBuffer);
    scRender.m_spriteShaderID = RendererUtils::InvalidID;
    g_assetManager->Release(scRender.m_spriteShaderAsset);
}