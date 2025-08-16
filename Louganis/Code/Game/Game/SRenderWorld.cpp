// Bradley Christensen - 2023
#include "SRenderWorld.h"
#include "SCWorld.h"
#include "Chunk.h"
#include "CCamera.h"
#include "Engine/Renderer/Renderer.h"
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

    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(&chunk->m_vbo);

#if defined(_DEBUG)
    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(nullptr);
    g_renderer->DrawVertexBuffer(&chunk->m_debugVBO);
#endif
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Startup()
{
    AddWriteDependencies<SCWorld, Renderer>();
    AddReadDependencies<CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Run(SystemContext const& context)
{
    SCWorld& world = g_ecs->GetSingleton<SCWorld>();

    for (auto cameraIt = g_ecs->Iterate<CCamera>(context); cameraIt.IsValid(); ++cameraIt)
    {
        CCamera* cameraComponent = g_ecs->GetComponent<CCamera>(cameraIt.m_currentIndex);
        AABB2 cameraOrthoBounds2D = cameraComponent->m_camera.GetOrthoBounds2D();
        cameraOrthoBounds2D.Translate(cameraComponent->m_camera.GetPosition2D());

        for (auto& chunkIt : world.m_activeChunks)
        {
            Chunk* chunk = chunkIt.second;
            if (chunk->m_chunkBounds.IsOverlapping(cameraOrthoBounds2D))
            {
                DrawChunk(chunk);
            }
        }

    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderWorld::Shutdown()
{

}