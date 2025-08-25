// Bradley Christensen - 2023
#include "SInitView.h"
#include "CCamera.h"
#include "Engine/Renderer/RendererInterface.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Startup()
{
    AddReadDependencies<CCamera>();
    AddWriteDependencies<RendererInterface>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const& context)
{
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();

    auto camIt = g_ecs->Iterate<CCamera>(context);
    if (camIt.IsValid())
    {
        CCamera& camera = cameraStorage[camIt];
        g_rendererInterface->BeginCameraAndWindow(&camera.m_camera, g_window);
        g_rendererInterface->ClearScreen(Rgba8::LightGray);
    }
}
