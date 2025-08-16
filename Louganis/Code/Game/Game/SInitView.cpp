// Bradley Christensen - 2023
#include "SInitView.h"
#include "CTransform.h"
#include "CRender.h"
#include "CCamera.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Startup()
{
    AddReadDependencies<CTransform, CCamera>();
    AddWriteDependencies<CRender, Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const& context)
{
    UNUSED(context)

    for (auto cameraIt = g_ecs->Iterate<CCamera>(context); cameraIt.IsValid(); ++cameraIt)
    {
        CCamera* cameraComponent = g_ecs->GetComponent<CCamera>(cameraIt.m_currentIndex);
        AABB2 cameraOrthoBounds2D = cameraComponent->m_camera.GetOrthoBounds2D();
        cameraOrthoBounds2D.Translate(cameraComponent->m_camera.GetPosition2D());

        g_renderer->BeginCameraAndWindow(&cameraComponent->m_camera, g_window);
        g_renderer->ClearScreen(Rgba8::White);
    }
}
