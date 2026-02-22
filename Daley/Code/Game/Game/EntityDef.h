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
    static void Shutdown();
    static EntityDef const* GetEntityDef(uint8_t id);
    static EntityDef const* GetEntityDef(Name name);
    static int GetEntityDefID(Name name);

private:

    static std::vector<EntityDef> s_entityDefs;

public:

    Name m_name = "Unnamed EntityDef";

    // Components
    std::optional<CAIController>        m_ai;
	std::optional<CAnimation>           m_animation;
	std::optional<CCollision>           m_collision;
	std::optional<CDeath>               m_death;
    std::optional<CHealth>              m_health;
	std::optional<CLifetime>			m_lifetime;
    std::optional<CMovement>            m_movement;
	std::optional<CRender>              m_render;
    std::optional<CTime>                m_time;
    std::optional<CTransform>           m_transform;
    std::optional<CWeapon>              m_weapon;
    std::optional<CProjectile>          m_proj;
    std::optional<CTags>                m_tags;
};
    