// Bradley Christensen - 2022-2026
#include "SInitView.h"
#include "CRender.h"
#include "SCCamera.h"
#include "SCRenderer.h"
#include "SCWindow.h"
#include "WorldSettings.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Startup()
{
    AddReadDependencies<SCCamera>();
    AddWriteDependencies<CRender, SCRenderer, SCWindow>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCCamera const& camera = context.GetSingletonConst<SCCamera>();

	// Write Dependencies
	auto& renderStorage = context.GetArrayStorage<CRender>();
	Renderer& renderer = *context.GetSingleton<SCRenderer>().GetRenderer();
	Window* window = context.GetSingleton<SCWindow>().GetWindow();

    AABB2 cameraBounds = camera.m_camera.GetTranslatedOrthoBounds2D();

	renderer.BeginWindowLetterboxed(window, StaticWorldSettings::s_visibleWorldAspect);
    renderer.BeginCamera(&camera.m_camera);
    renderer.ClearScreen(Rgba8::LightGray);

    for (auto it = context.Iterate<CRender>(); it.IsValid(); ++it)
    {
        CRender& render = renderStorage[it];

        bool isInView = GeometryUtils::DoesDiscOverlapAABB(render.GetRenderPosition(), render.m_renderRadius, cameraBounds);
        render.SetIsInCameraView(isInView);
    }
}
