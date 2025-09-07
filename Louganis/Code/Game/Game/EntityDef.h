// Bradley Christensen - 2022-2025
#pragma once
#include "AllComponents.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Core/Name.h"
#include <vector>
#include <optional>



//----------------------------------------------------------------------------------------------------------------------
struct EntityDef
{
public:

    explicit EntityDef(XmlElement const* xmlElement);

    static void LoadFromXML();
    static EntityDef const* GetEntityDef(uint8_t id);
    static EntityDef const* GetEntityDef(Name name);
    static int GetEntityDefID(Name name);

private:

    static std::vector<EntityDef> s_entityDefs;

public:

    Name m_name = "Unnamed EntityDef";

    // Components
    std::optional<CAbility>             m_ability;
	std::optional<CAnimation>           m_animation;
    std::optional<CCamera>              m_camera;
	std::optional<CCollision>           m_collision;
    std::optional<CPlayerController>    m_playerController;
    std::optional<CMovement>            m_movement;
	std::optional<CRender>              m_render;
    std::optional<CTransform>           m_transform;
};
