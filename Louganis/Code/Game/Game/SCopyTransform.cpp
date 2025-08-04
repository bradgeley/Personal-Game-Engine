﻿// Bradley Christensen - 2023
#include "SCopyTransform.h"
#include "CRender.h"
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_Z_LAYERS = 2;



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
        CRender& render = *renderStorage.Get(renderIt.m_currentIndex);
        CTransform& transform = *transStorage.Get(renderIt.m_currentIndex);

        render.m_pos = transform.m_pos;
        render.m_orientation = transform.m_orientation;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SCopyTransform::Shutdown()
{

}