// Bradley Christensen - 2022-2026
#include "ProjectileHitAbilityDef.h"
#include "Ability.h"
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



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbilityDef::WriteToXmlDoc(void* xmlDoc, void* rootElement)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "ProjectileHitAbilityDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(rootElement != nullptr, "ProjectileHitAbilityDef::WriteToXmlDoc - rootElement is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& rootElem = *static_cast<XmlElement*>(rootElement);

    // Ability
	XmlElement* parentElem = doc.NewElement("ProjectileHitAbilityDef");
	parentElem->SetAttribute("name", m_name.ToCStr());
	parentElem->SetAttribute("projectileDef", m_projectileDefName.ToCStr());
	parentElem->SetAttribute("projSpeed", m_projSpeed);
    rootElem.InsertEndChild(parentElem);

	m_cooldownDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_targetingDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_critDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_chainDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_multishotDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_onHitDef.WriteToXmlDoc(xmlDoc, parentElem);
}



//----------------------------------------------------------------------------------------------------------------------
void ProjectileHitAbilityDef::SwirlInto(AbilityDef& other) const
{
    other.ReceiveSwirl(*this);
}



//----------------------------------------------------------------------------------------------------------------------
bool ProjectileHitAbilityDef::ReceiveSwirl(ProjectileHitAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool ProjectileHitAbilityDef::ReceiveSwirl(AoEHitAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool ProjectileHitAbilityDef::ReceiveSwirl(PassiveAoEAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool ProjectileHitAbilityDef::ReceiveSwirl(LaserAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool ProjectileHitAbilityDef::ReceiveSwirl(AdjacentHitAbilityDef const& other)
{
    return false;
}
