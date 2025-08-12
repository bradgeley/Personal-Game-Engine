// Bradley Christensen - 2023
#include "SInitView.h"
#include "CTransform.h"
#include "CRender.h"
#include "Engine/Core/EngineCommon.h"



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Startup()
{
    AddReadDependencies<CTransform>();
    AddWriteDependencies<CRender>();
}



//----------------------------------------------------------------------------------------------------------------------
void SInitView::Run(SystemContext const& context)
{
    UNUSED(context)
    //auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
    //auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    //
    //for (GroupIter it = g_ecs->Iterate<CRender, CTransform>(context); it.IsValid(); ++it)
    //{
    //    CRender* render = renderStorage.Get(it);
    //    CTransform* transform = transStorage.Get(it);
    //
    //
    //}
}
