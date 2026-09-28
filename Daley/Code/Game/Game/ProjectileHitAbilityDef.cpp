// Bradley Christensen - 2022-2026
#include "ProjectileHitAbilityDef.h"
#include "Ability.h"
#include "ProjectileHitAbility.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitAbilityDef::ProjectileHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_projectileDefName = XmlUtils::ParseXmlAttribute(elem, "projectileDef", m_projectileDefName);
    m_projSpeed = XmlUtils::ParseXmlAttribute(elem, "projSpeed", m_projSpeed);

    if (XmlElement const* cooldownElem = elem.FirstChildElement("Cooldown"))
    {
        m_cooldownDef = AbilityCooldownComponentDef(cooldownElem);
    }
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef = AbilityTargetingComponentDef(targetingElem);
    }
    if (XmlElement const* critElem = elem.FirstChildElement("Crit"))
    {
        m_critDef = AbilityCritComponentDef(critElem);
    }
    if (XmlElement const* onHitElem = elem.FirstChildElement("OnHit"))
    {
        m_onHitDef = AbilityOnHitComponentDef(onHitElem);
    }
    if (XmlElement const* chainElem = elem.FirstChildElement("Chain"))
    {
        m_chainDef = AbilityChainComponentDef(chainElem);
    }
    if (XmlElement const* multishotElem = elem.FirstChildElement("Multishot"))
    {
        m_multishotDef = AbilityMultishotComponentDef(multishotElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* ProjectileHitAbilityDef::MakeAbilityInstance() const
{
    ProjectileHitAbility* ability = new ProjectileHitAbility(*this);
    return ability;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* ProjectileHitAbilityDef::Copy() const
{
    return new ProjectileHitAbilityDef(*this);
}