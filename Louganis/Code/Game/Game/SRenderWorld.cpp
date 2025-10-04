// Bradley Christensen - 2022-2025
#include "SRenderWorld.h"
#include "SCCamera.h"
#include "Chunk.h"
#include "SCRender.h"
#include "SCWorld.h"
#include "WorldShaderCPU.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void DrawChunk(Chunk* chunk);



//----------------------------------------------------------------------------------------------------------------------
void DrawChunk(Chunk* chunk)
{
    if (!chunk)
    {
        return;
    }

    chunk->GenerateVBO();

	g_renderer->BindTexture(chunk->m_lightmap, 1);
    g_renderer->DrawVertexBuffer(chunk->m_vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCRender, SCWorld, Renderer>();
    AddReadDependencies<SCCamera, AssetManager>();

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	scRender.m_staticWorldConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(StaticWorldConstants));

    StaticWorldConstants worldConstants;
	worldConstants.m_chunkWidth = StaticWorldSettings::s_chunkWidth;
	worldConstants.m_numTilesInRow = StaticWorldSettings::s_numTilesInRow;
	worldConstants.m_tileWidth = StaticWorldSettings::s_tileWidth;

	ConstantBuffer* worldConstantsBuffer = g_renderer->GetConstantBuffer(scRender.m_staticWorldConstantsBuffer);
    worldConstantsBuffer->Update(worldConstants);

	scRender.m_worldShaderAsset = g_assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/WorldShader.xml");

    // This is mostly to ref count the sheet so it stays loaded
    SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.m_worldSpriteSheet = g_assetManager->AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	ASSERT_OR_DIE(scWorld.m_worldSpriteSheet != AssetID::Invalid, "Failed to load world sprite sheet in SRenderWorld::Startup");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const&)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	SCCamera& scCamera = g_ecs->GetSingleton<SCCamera>();

	GridSpriteSheet* worldSpriteSheet = g_assetManager->Get<GridSpriteSheet>(world.m_worldSpriteSheet);
    if (worldSpriteSheet)
    {
        worldSpriteSheet->SetRendererState();
    }
    else
    {
        return;
    }

	ShaderAsset* shaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_worldShaderAsset);
    if (shaderAsset)
    {
		scRender.m_worldShaderID = shaderAsset->GetShaderID();
    }
    else
    {
		scRender.m_worldShaderID = RendererUtils::InvalidID;
        return;
    }

    g_renderer->BindShader(scRender.m_worldShaderID);
	g_renderer->BindConstantBuffer(scRender.m_staticWorldConstantsBuffer, StaticWorldConstants::GetSlot());
	g_renderer->BindConstantBuffer(scRender.m_lightingConstantsBuffer, LightingConstants::GetSlot());

    AABB2 cameraOrthoBounds2D = scCamera.m_camera.GetTranslatedOrthoBounds2D();

    world.ForEachChunkOverlappingAABB(cameraOrthoBounds2D, [worldSpriteSheet](Chunk& chunk)
    {
        DrawChunk(&chunk);
        return true;
    });
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown()
{
    auto& scWorld = g_ecs->GetSingleton<SCWorld>();
	g_assetManager->Release(scWorld.m_worldSpriteSheet);

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	g_assetManager->Release(scRender.m_worldShaderAsset);
	g_renderer->ReleaseConstantBuffer(scRender.m_spriteSheetConstantsBuffer);
}
