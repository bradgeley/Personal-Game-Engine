// Bradley Christensen - 2023
#include "EntityDef.h"



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
        //m_camera.emplace(CCamera(elem));
        m_camera = CCamera(elem);
    }

    // CMovement
    elem = xmlElement->FirstChildElement("Movement");
    if (elem)
    {
        //m_movement.emplace(CMovement(elem));
        m_movement = CMovement(elem);
    }

    // CPhysics
    elem = xmlElement->FirstChildElement("Physics");
    if (elem)
    {
        //m_physics.emplace(CPhysics(elem));
        m_physics = CPhysics(elem);
    }

    // CRender
    elem = xmlElement->FirstChildElement("Render");
    if (elem)
    {
        //m_render.emplace(CRender(elem));
        m_render = CRender(elem);
    }
}
