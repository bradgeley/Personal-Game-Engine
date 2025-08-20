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
    AddWriteDependencies<Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const& context)
{
    UNUSED(context)

    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();

    g_renderer->ClearScreen(Rgba8::LightGray);

    auto camIt = g_ecs->Iterate<CCamera>(context);
    if (camIt.IsValid())
    {
        CCamera& camera = cameraStorage[camIt.m_currentIndex];
        g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
    }
}
