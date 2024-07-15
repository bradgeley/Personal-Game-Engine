// Bradley Christensen - 2023
#include "SRenderEntities.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "CRender.h"
#include "CCamera.h"
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Startup()
{
    AddWriteDependencies<CRender, Renderer>();
    AddReadDependencies<CTransform, CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context)
{
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

    VertexBuffer vbo;
    AddVertsForAABB2(vbo.GetMutableVerts(), AABB2(-0.5, -0.5, 0.5, 0.5));

    for (auto camIt = g_ecs->Iterate<CCamera>(context); camIt.IsValid(); ++camIt)
    {
        // For each camera
        CCamera& camera = cameraStorage[camIt.m_currentIndex];
        g_renderer->BeginCamera(&camera.m_camera);

        // Render all things
        ModelConstants modelConstants;
        for (auto renderIt = g_ecs->Iterate<CRender>(context); renderIt.IsValid(); ++renderIt)
        {
            CRender& render = *renderStorage.Get(renderIt.m_currentIndex);
            modelConstants.m_modelMatrix.Reset();
            modelConstants.m_modelMatrix.AppendZRotation(render.m_orientation);
            modelConstants.m_modelMatrix.AppendTranslation2D(render.m_pos);
            render.m_tint.GetAsFloats(modelConstants.m_modelRgba);

            g_renderer->SetModelConstants(modelConstants);
            g_renderer->BindShader(nullptr);
            g_renderer->BindTexture(nullptr);
            g_renderer->DrawVertexBuffer(&vbo);
        }

        g_renderer->EndCamera(&camera.m_camera);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown()
{
    
}