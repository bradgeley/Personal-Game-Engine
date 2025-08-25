// Bradley Christensen - 2023
#include "SRenderEntities.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "CRender.h"
#include "CCamera.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Startup()
{
    AddWriteDependencies<CRender, Renderer>();
    AddReadDependencies<CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context)
{
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();

    VertexBuffer* vbo = g_renderer->MakeVertexBuffer();
    AddVertsForDisc2D(vbo->GetMutableVerts(), Vec2(), 1, 128);

    // Render all things (1 draw call per entity = bad, todo: write sprite geometry shader)
    ModelConstants modelConstants;
    for (auto renderIt = g_ecs->Iterate<CRender>(context); renderIt.IsValid(); ++renderIt)
    {
        const CRender& render = *renderStorage.Get(renderIt);
        modelConstants.m_modelMatrix.Reset();
        //modelConstants.m_modelMatrix.AppendZRotation(render.m_orientation);
        modelConstants.m_modelMatrix.AppendTranslation2D(render.m_pos);
        modelConstants.m_modelMatrix.AppendUniformScale2D(render.m_scale);
        render.m_tint.GetAsFloats(modelConstants.m_modelRgba);

        g_renderer->SetModelConstants(modelConstants);
        g_renderer->BindShader(nullptr);
        g_renderer->BindTexture(nullptr);
        g_renderer->DrawVertexBuffer(vbo);
    }

    g_renderer->SetModelConstants(ModelConstants());

    vbo->ReleaseResources();
    delete vbo;
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown()
{
    
}