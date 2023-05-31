// Bradley Christensen - 2023
#include "SCamera.h"
#include "Game/Game/Components/CCamera.h"
#include "Game/Game/Components/CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
void SCamera::Run(SystemContext const& context)
{
    for (auto it = g_ecs->Iterate<CTransform, CCamera>(context); it.IsValid(); ++it)
    {
        CTransform& transform = *g_ecs->GetComponent<CTransform>(it.m_currentIndex);
        CCamera& camera = *g_ecs->GetComponent<CCamera>(it.m_currentIndex);

        camera.m_camera.SetOrthoDims2D(camera.m_baseDims * camera.m_zoom);
        camera.m_camera.SetOrthoCenter2D(transform.m_pos);
    }
}
