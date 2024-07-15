// Bradley Christensen - 2023
#pragma once
#include "AllComponents.h"
#include "Engine/Core/XmlUtils.h"
#include <vector>
#include <optional>



//----------------------------------------------------------------------------------------------------------------------
struct EntityDef
{
public:

    explicit EntityDef(XmlElement const* xmlElement);

    static void LoadFromXML();
    static EntityDef const* GetEntityDef(uint8_t id);
    static EntityDef const* GetEntityDef(std::string const& name);
    static uint8_t GetEntityDefID(std::string const& name);

private:

    static std::vector<EntityDef> s_entityDefs;

public:

    std::string m_name = "Unnamed EntityDef";

    // Components
    std::optional<CTransform>           m_transform;
    std::optional<CCamera>              m_camera;
    std::optional<CMovement>            m_movement;
    std::optional<CPlayerController>    m_playerController;
	std::optional<CRender>              m_render;
	std::optional<CCollision>           m_collision;
};
