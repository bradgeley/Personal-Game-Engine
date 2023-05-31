// Bradley Christensen - 2023
#pragma once
#include "AllComponents.h"
#include "Engine/Core/XmlUtils.h"
#include <optional>



//----------------------------------------------------------------------------------------------------------------------
struct EntityDef
{
    explicit EntityDef(XmlElement const* xmlElement);

    // Components
    std::optional<CTransform>   m_transform;
    std::optional<CCamera>      m_camera;
    std::optional<CMovement>    m_movement;
    std::optional<CPhysics>     m_physics;
    std::optional<CRender>      m_render;
};
