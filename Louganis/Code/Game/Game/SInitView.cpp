// Bradley Christensen - 2022-2025
#include "SInitView.h"
#include "SCCamera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Startup()
{
    AddReadDependencies<SCCamera>();
    AddWriteDependencies<Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const&)
{
	SCCamera& camera = g_ecs->GetSingleton<SCCamera>();
    g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
    g_renderer->ClearScreen(Rgba8::LightGray);
}
