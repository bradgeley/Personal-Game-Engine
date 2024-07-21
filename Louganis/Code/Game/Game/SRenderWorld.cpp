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

    for (auto it = g_ecs->Iterate<CCamera>(context); it.IsValid(); ++it)
    {
        CCamera* cameraComponent = g_ecs->GetComponent<CCamera>(it.m_currentIndex);
        AABB2 cameraOrthoBounds2D = cameraComponent->m_camera.GetOrthoBounds2D();
        cameraOrthoBounds2D.Translate(cameraComponent->m_camera.GetPosition2D());

        g_renderer->BeginCameraAndWindow(&cameraComponent->m_camera, g_window);
        g_renderer->ClearScreen(Rgba8::White);

        for (auto& it : world.m_activeChunks)
        {
            Chunk* chunk = it.second;
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