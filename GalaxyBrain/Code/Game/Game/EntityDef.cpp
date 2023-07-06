// Bradley Christensen - 2023
#include "EntityDef.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Renderer/Texture.h"



//----------------------------------------------------------------------------------------------------------------------
EntityDef::EntityDef(XmlElement const* xmlElement)
{
    // CTransform
    auto elem = xmlElement->FirstChildElement("Transform");
    if (elem)
    {
        m_transform.emplace(CTransform());
    }

    // CCamera
    elem = xmlElement->FirstChildElement("Camera");
    if (elem)
    {
        m_camera = CCamera(elem);
    }

    // CMovement
    elem = xmlElement->FirstChildElement("Movement");
    if (elem)
    {
        m_movement = CMovement(elem);
    }

    // CPhysics
    elem = xmlElement->FirstChildElement("Physics");
    if (elem)
    {
        m_physics = CPhysics(elem);
    }

    // CCollision
    elem = xmlElement->FirstChildElement("Collision");
    if (elem)
    {
        m_collision = CCollision(elem);
    }

    // CRender
    elem = xmlElement->FirstChildElement("Render");
    if (elem)
    {
        m_render = CRender(elem);
    }

    // CPlayerController
    elem = xmlElement->FirstChildElement("PlayerController");
    if (elem)
    {
        m_playerController = CPlayerController(elem);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void EntityDef::Cleanup()
{
    // Render
    if (m_render.has_value() && m_render->m_texture)
    {
        m_render->m_texture->ReleaseResources();
        delete m_render->m_texture;
        m_render->m_texture = nullptr;
    }
}
