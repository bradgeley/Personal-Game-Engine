// Bradley Christensen - 2023
#include "SInitView.h"
#include "CCamera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Startup()
{
    AddReadDependencies<CCamera>();
    AddWriteDependencies<Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const& context)
{
    auto& cameraStorage = g_ecs->GetMapStorage<CCamera>();

    g_renderer->ClearScreen(Rgba8::LightGray);

    auto camIt = g_ecs->Iterate<CCamera>(context);
    if (camIt.IsValid())
    {
        CCamera& camera = cameraStorage[camIt];
        g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
    }
}
