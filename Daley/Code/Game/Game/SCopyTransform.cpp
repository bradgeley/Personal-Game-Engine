// Bradley Christensen - 2022-2026
#include "SCopyTransform.h"
#include "CRender.h"
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SCopyTransform::Startup()
{
    AddReadDependencies<CTransform>();
    AddWriteDependencies<CRender>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCopyTransform::Run(SystemContext const& context) const
{
	// Read Dependencies
    auto& transStorage = context.GetArrayStorageConst<CTransform>();

	// Write Dependencies
    auto& renderStorage = context.GetArrayStorage<CRender>();

    for (auto it = context.Iterate<CRender, CTransform>(); it.IsValid(); ++it)
    {
        CRender& render = renderStorage[it];
        CTransform const& transform = transStorage[it];

        render.m_pos = transform.m_pos;
        render.m_orientation = transform.m_orientation;
    }
}