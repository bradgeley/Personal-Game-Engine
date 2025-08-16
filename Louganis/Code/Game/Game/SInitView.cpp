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

    g_renderer->ClearScreen(Rgba8::LightGray);
}
