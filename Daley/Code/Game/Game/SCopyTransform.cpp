// Bradley Christensen - 2022-2026
#include "SCopyTransform.h"
#include "CRender.h"
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SCopyTransform::Startup()
{
    AddWriteDependencies<CRender>();
    AddReadDependencies<CTransform>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCopyTransform::Run(SystemContext const& context)
{
    auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
    auto& transStorage = g_ecs->GetArrayStorage<CTransform>();

    for (auto renderIt = g_ecs->Iterate<CRender, CTransform>(context); renderIt.IsValid(); ++renderIt)
    {
        CRender& render = *renderStorage.Get(renderIt);
        CTransform const& transform = *transStorage.Get(renderIt);

        render.m_pos = transform.m_pos;
        render.m_orientation = transform.m_orientation;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCopyTransform::Shutdown()
{

}