// Bradley Christensen - 2023
#include "SRenderWorld.h"
#include "SCWorld.h"
#include "Chunk.h"
#include "CCamera.h"
#include "Engine/Renderer/RendererInterface.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void DrawChunk(Chunk* chunk);



//----------------------------------------------------------------------------------------------------------------------
void DrawChunk(Chunk* chunk)
{
    if (!chunk)
    {
        return;
    }

    g_rendererInterface->BindTexture(nullptr);
    g_rendererInterface->BindShader(nullptr);
    g_rendererInterface->DrawVertexBuffer(chunk->m_vbo);

#if defined(_DEBUG)
    g_rendererInterface->BindTexture(nullptr);
    g_rendererInterface->BindShader(nullptr);
    g_rendererInterface->DrawVertexBuffer(chunk->m_debugVBO);
#endif
}



//----------------------------------------------------------- -----------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCWorld, RendererInterface>();
    AddReadDependencies<CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const& context)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    auto cameraIt = g_ecs->Iterate<CCamera>(context);
    if (cameraIt.IsValid())
    {
        CCamera* cameraComponent = g_ecs->GetComponent<CCamera>(cameraIt);
        AABB2 cameraOrthoBounds2D = cameraComponent->m_camera.GetOrthoBounds2D();
        cameraOrthoBounds2D.Translate(cameraComponent->m_camera.GetPosition2D());

        if (cameraOrthoBounds2D.GetHeight() > world.m_worldSettings.m_chunkLoadRadius)
        {
            for (auto& it : world.m_activeChunks)
            {
                if (it.second->m_chunkBounds.IsOverlapping(cameraOrthoBounds2D))
                {
                    DrawChunk(it.second);
                }
            }
        }
        else
        {
            // This is faster if the camera bounds are small, slower (uncapped) if bounds are much bigger than the visible world.
            world.ForEachChunkOverlappingAABB(cameraOrthoBounds2D, [](Chunk& chunk)
            {
                DrawChunk(&chunk);
                return true;
            });
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown()
{

}