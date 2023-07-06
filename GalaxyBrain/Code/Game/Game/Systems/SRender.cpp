// Bradley Christensen - 2023
#include "SRender.h"
#include "Game/Game/Components/CRender.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"
#include "Game/Game/Components/CCamera.h"
#include "Game/Game/Singletons/SCRenderer.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_Z_LAYERS = 2;



//----------------------------------------------------------------------------------------------------------------------
void SRender::Startup()
{
    AddWriteDependencies<CRender, Renderer>();

    auto& scRender = *g_ecs->GetComponent<SCRenderer>();

    VertexBuffer& vbo1 = scRender.m_vbos[(int) GameVboIndex::Sprite];
    AddVertsForRect2D(vbo1.GetMutableVerts(), Vec2(-5.f, -10.f), Vec2(5.f, 10.f));

    VertexBuffer& vbo2 = scRender.m_vbos[(int)GameVboIndex::Sphere];
    AddVertsForDisc2D(vbo2.GetMutableVerts(), Vec2(0.f, 0.f), 100.f, 64);
}



//----------------------------------------------------------------------------------------------------------------------
void SRender::Run(SystemContext const& context)
{
    auto& scRender = *g_ecs->GetComponent<SCRenderer>();
    
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

    // Todo: move this part out to a separate system - Copy transform or something
    for (auto renderIt = g_ecs->Iterate<CRender, CTransform>(context); renderIt.IsValid(); ++renderIt)
    {
        CRender& render = *renderStorage.Get(renderIt.m_currentIndex);
        CTransform& trans = *transStorage.Get(renderIt.m_currentIndex);
        render.m_renderTransform.m_pos = trans.m_pos;
        render.m_modelConstants.m_modelMatrix.Reset();
        render.m_modelConstants.m_modelMatrix.AppendZRotation(render.m_renderTransform.m_orientation);
        render.m_modelConstants.m_modelMatrix.SetTranslation2D(render.m_renderTransform.m_pos);
    }

    for (auto camIt = g_ecs->Iterate<CCamera>(context); camIt.IsValid(); ++camIt)
    {
        // For each camera
        CCamera& camera = *cameraStorage.Get(camIt.m_currentIndex);
        g_renderer->BeginCamera(camera.m_camera);

        // Render all things
        for (int i = 0; i < MAX_Z_LAYERS; ++i)
        {
            for (auto renderIt = g_ecs->Iterate<CRender>(context); renderIt.IsValid(); ++renderIt)
            {
                CRender& render = *renderStorage.Get(renderIt.m_currentIndex);

                if (render.m_layer == i)
                {
                    g_renderer->SetModelConstants(render.m_modelConstants);
                    g_renderer->BindShader(nullptr);
                    g_renderer->BindTexture(render.m_texture);
                    g_renderer->DrawVertexBuffer(&scRender.m_vbos[render.m_vboIndex]);
                }
            }
        }

    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRender::Shutdown()
{

}


