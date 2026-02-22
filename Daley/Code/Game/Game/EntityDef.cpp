// Bradley Christensen - 2022-2025
#include "EntityDef.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Renderer/Texture.h"



//----------------------------------------------------------------------------------------------------------------------
const char* s_entityDefsFilePath = "Data/Definitions/EntityDefs.xml";
std::vector<EntityDef> EntityDef::s_entityDefs;



//----------------------------------------------------------------------------------------------------------------------
void EntityDef::LoadFromXML()
{
    XmlDocument doc;
    doc.LoadFile(s_entityDefsFilePath);
    auto root = doc.RootElement();
    if (!root)
    {
        g_devConsole->LogErrorF("SEntityFactory::LoadFromXml - Could not load file: %s", s_entityDefsFilePath);
        return; 
    }

    XmlElement* entityDefElem = root->FirstChildElement("EntityDef");
    while (entityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*entityDefElem, "name", Name::Invalid);
        if (GetEntityDefID(name) != -1)
        {
            g_devConsole->LogErrorF("Duplicate Entity Def: %s", name.ToCStr());
        }

        // Emplace new definition using the constructor that takes an Xml Element
        s_entityDefs.emplace_back(entityDefElem);

        entityDefElem = entityDefElem->NextSiblingElement("EntityDef");
    }
}



//----------------------------------------------------------------------------------------------------------------------
void EntityDef::Shutdown()
{
    s_entityDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
EntityDef const* EntityDef::GetEntityDef(uint8_t id)
{
    size_t index = static_cast<size_t>(id);
    return &s_entityDefs[index];
}



//----------------------------------------------------------------------------------------------------------------------
EntityDef const* EntityDef::GetEntityDef(Name name)
{
    for (size_t i = 0; i < s_entityDefs.size(); i++)
    {
        EntityDef const* def = &s_entityDefs[i];
        if (def->m_name == name)
        {
            return def;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
int EntityDef::GetEntityDefID(Name name)
{
    for (size_t i = 0; i < s_entityDefs.size(); i++)
    {
        EntityDef const* def = &s_entityDefs[i];
        if (def->m_name == name)
        {
            return (int) i;
        }
    }
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
EntityDef::EntityDef(XmlElement const* xmlElement)
{
    m_name = XmlUtils::ParseXmlAttribute(*xmlElement, "name", m_name);
    g_devConsole->AddLine(StringUtils::StringF("Entity def loading: %s", m_name.ToCStr()));

    // CTime
    auto elem = xmlElement->FirstChildElement("Time");
    if (elem)
    {
        m_time.emplace();
    }

    // CTransform
    elem = xmlElement->FirstChildElement("Transform");
    if (elem)
    {
        m_transform.emplace();
    }

    // CAIController
    elem = xmlElement->FirstChildElement("AIController");
    if (elem)
    {
        m_ai.emplace(elem);
    }

    // CAnimation
    elem = xmlElement->FirstChildElement("Animation");
    if (elem)
    {
        m_animation.emplace(elem);
    }

    // CCollision
    elem = xmlElement->FirstChildElement("Collision");
    if (elem)
    {
        m_collision.emplace(elem);
    }

    // CMovement
    elem = xmlElement->FirstChildElement("Movement");
    if (elem)
    {
        m_movement.emplace(elem);
    }

    // CRender
    elem = xmlElement->FirstChildElement("Render");
    if (elem)
    {
        m_render.emplace(elem);
    }

    // CLifetime
    elem = xmlElement->FirstChildElement("Lifetime");
    if (elem)
    {
        m_lifetime.emplace(elem);
    }

    // CHealth
    elem = xmlElement->FirstChildElement("Health");
    if (elem)
    {
        m_health.emplace(elem);
    }

    // CDeath
    elem = xmlElement->FirstChildElement("Death");
    if (elem)
    {
        m_death.emplace(elem);
    }

    // CWeapon
    elem = xmlElement->FirstChildElement("Weapon");
    if (elem)
    {
        m_weapon.emplace(elem);
    }

    // CProjectile
    elem = xmlElement->FirstChildElement("Projectile");
    if (elem)
    {
        m_proj.emplace(elem);
    }

    // CTags
    elem = xmlElement->FirstChildElement("Tags");
    if (elem)
    {
        m_tags.emplace(elem);
    }
}
