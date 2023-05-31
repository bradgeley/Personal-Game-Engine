// Bradley Christensen - 2023
#include "SRender.h"
#include "Game/Game/Components/CRender.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Game/Game/Components/CCamera.h"
#include "Game/Game/Singletons/SCRenderer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRender::Startup()
{
    AddWriteDependencies<CRender, Renderer>();

    auto& scRender = *g_ecs->GetComponent<SCRenderer>();

    VertexBuffer& vbo1 = scRender.m_vbos[(int) GameVboIndex::Player];
    AddVertsForRect2D(vbo1.GetMutableVerts(), Vec2(-5.f, -10.f), Vec2(5.f, 10.f));
}



//----------------------------------------------------------------------------------------------------------------------
void SRender::Run(SystemContext const& context)
{
    g_renderer->ClearScreen(Rgba8::Black);
    
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
        CCamera& camera = *cameraStorage.Get(camIt.m_currentIndex);
        g_renderer->BeginCamera(camera.m_camera);
        
        for (auto renderIt = g_ecs->Iterate<CRender>(context); renderIt.IsValid(); ++renderIt)
        {
            CRender& render = *renderStorage.Get(renderIt.m_currentIndex);
            g_renderer->SetModelConstants(render.m_modelConstants);
            g_renderer->DrawVertexBuffer(&scRender.m_vbos[render.m_vboIndex]);
        }
    }

}
