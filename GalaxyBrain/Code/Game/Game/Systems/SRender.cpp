// Bradley Christensen - 2023
#include "SRender.h"
#include "Game/Game/Components/CRender.h"
#include "Engine/Renderer/Renderer.h"
#include "Game/Game/Components/CCamera.h"



//----------------------------------------------------------------------------------------------------------------------
void SRender::Startup()
{
    AddWriteDependencies<CRender, Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRender::Run(SystemContext const& context)
{
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();

    for (auto renderIt = g_ecs->Iterate<CRender>(context); renderIt.IsValid(); ++renderIt)
    {
        CRender& render = *renderStorage.Get(renderIt.m_currentIndex);
        render.m_modelConstants.m_modelMatrix = Mat44();
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
            g_renderer->DrawVertexBuffer(&render.m_vertexBuffer);
        }
    }

}
