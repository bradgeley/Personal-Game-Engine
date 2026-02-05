// Bradley Christensen - 2022-2025
#include "SInitView.h"
#include "CRender.h"
#include "SCCamera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Window/Window.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Startup()
{
    AddReadDependencies<SCCamera>();
    AddWriteDependencies<CRender, Renderer>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const& context)
{
	SCCamera const& camera = g_ecs->GetSingleton<SCCamera>();
    AABB2 cameraBounds = camera.m_camera.GetTranslatedOrthoBounds2D();

    g_renderer->BeginCameraAndWindow(&camera.m_camera, g_window);
    g_renderer->ClearScreen(Rgba8::LightGray);

	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();

    for (auto it = g_ecs->Iterate<CRender>(context); it.IsValid(); ++it)
    {
        CRender& render = renderStorage[it];

        bool isInView = GeometryUtils::DoesDiscOverlapAABB(render.GetRenderPosition(), render.m_renderRadius, cameraBounds);
        render.SetIsInCameraView(isInView);
    }
}
