// Bradley Christensen - 2023
#include "SRenderEntities.h"
#include "Engine/Renderer/RendererInterface.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "CRender.h"
#include "CCamera.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Startup()
{
    AddWriteDependencies<CRender, RendererInterface>();
    AddReadDependencies<CCamera>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Run(SystemContext const& context)
{
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();

    VertexBuffer* vbo = g_rendererInterface->MakeVertexBuffer();
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

        g_rendererInterface->SetModelConstants(modelConstants);
        g_rendererInterface->BindShader(nullptr);
        g_rendererInterface->BindTexture(nullptr);
        g_rendererInterface->DrawVertexBuffer(vbo);
    }

    g_rendererInterface->SetModelConstants(ModelConstants());

    delete vbo;
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderEntities::Shutdown()
{
    
}