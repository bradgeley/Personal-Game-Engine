// Bradley Christensen - 2023
#pragma once
#include "AllComponents.h"
#include "Engine/Core/XmlUtils.h"
#include <optional>



//----------------------------------------------------------------------------------------------------------------------
struct EntityDef
{
    EntityDef() = default;
    explicit EntityDef(XmlElement const* xmlElement);
    void Cleanup();

    // Components
    std::optional<CTransform>           m_transform;
    std::optional<CCamera>              m_camera;
    std::optional<CMovement>            m_movement;
    std::optional<CPhysics>             m_physics;
    std::optional<CPlayerController>    m_playerController;
	std::optional<CRender>              m_render;
	std::optional<CCollision>           m_collision;
};
