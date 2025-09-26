// Bradley Christensen - 2022-2025
#include "SRenderWorld.h"
#include "SCWorld.h"
#include "Chunk.h"
#include "CCamera.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/GridSpriteSheet.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void DrawChunk(Chunk* chunk, GridSpriteSheet* worldSpriteSheet);



//----------------------------------------------------------------------------------------------------------------------
void DrawChunk(Chunk* chunk, GridSpriteSheet* worldSpriteSheet)
{
    if (!chunk)
    {
        return;
    }

    #if defined(_DEBUG)
	    worldSpriteSheet->SetRendererState();
    #else
	    UNUSED(worldSpriteSheet);
    #endif

    chunk->GenerateVBO();
    g_renderer->DrawVertexBuffer(chunk->m_vbo);

    #if defined(_DEBUG)
        g_renderer->BindTexture(nullptr);
        g_renderer->BindShader(nullptr);
        g_renderer->DrawVertexBuffer(chunk->m_debugVBO);
    #endif
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCWorld, Renderer>();
    AddReadDependencies<CCamera, AssetManager>();

	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();

    // This is mostly to ref count the sheet so it stays loaded
	scWorld.m_worldSpriteSheet = g_assetManager->AsyncLoad<GridSpriteSheet>("Data/SpriteSheets/Terrain.xml");
	ASSERT_OR_DIE(scWorld.m_worldSpriteSheet != AssetID::Invalid, "Failed to load world sprite sheet in SRenderWorld::Startup");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const& context)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

	GridSpriteSheet* worldSpriteSheet = g_assetManager->Get<GridSpriteSheet>(world.m_worldSpriteSheet);
    if (worldSpriteSheet)
    {
        worldSpriteSheet->SetRendererState();
    }
    else
    {
        return;
    }

    auto cameraIt = g_ecs->Iterate<CCamera>(context);
    if (cameraIt.IsValid())
    {
        CCamera* cameraComponent = g_ecs->GetComponent<CCamera>(cameraIt);
        AABB2 cameraOrthoBounds2D = cameraComponent->m_camera.GetTranslatedOrthoBounds2D();

        if (cameraOrthoBounds2D.GetHeight() > world.m_worldSettings.m_chunkLoadRadius)
        {
            for (auto& it : world.m_activeChunks)
            {
                if (it.second->m_chunkBounds.IsOverlapping(cameraOrthoBounds2D))
                {
					Chunk* chunk = it.second;
                    DrawChunk(chunk, worldSpriteSheet);
                }
            }
        }
        else
        {
            // This is faster if the camera bounds are small, slower (uncapped) if bounds are much bigger than the visible world.
            world.ForEachChunkOverlappingAABB(cameraOrthoBounds2D, [worldSpriteSheet](Chunk& chunk)
            {
                DrawChunk(&chunk, worldSpriteSheet);
                return true;
            });
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown()
{
    auto scWorld = g_ecs->GetSingleton<SCWorld>();

	g_assetManager->Release(scWorld.m_worldSpriteSheet);
}
