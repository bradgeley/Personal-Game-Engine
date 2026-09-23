// Bradley Christensen - 2022-2026
#include "LaserAbilityDef.h"
#include "Ability.h"
#include "AdjacentHitAbilityDef.h"
#include "AoEHitAbilityDef.h"
#include "PassiveAoEAbilityDef.h"
#include "ProjectileHitAbilityDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
LaserAbilityDef::LaserAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef = AbilityTargetingComponentDef(targetingElem);
    }
    if (XmlElement const* onHitElem = elem.FirstChildElement("OnHit"))
    {
        m_onHitDef = AbilityOnHitComponentDef(onHitElem);
	}
    if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
    {
        m_renderDef = AbilityRenderComponentDef(renderElem);
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
Ability* LaserAbilityDef::MakeAbilityInstance() const
{
    LaserAbility* ability = new LaserAbility(*this);
	return ability;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* LaserAbilityDef::Copy() const
{
    return new LaserAbilityDef(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbilityDef::WriteToXmlDoc(void* xmlDoc, void* rootElement)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "LaserAbilityDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(rootElement != nullptr, "LaserAbilityDef::WriteToXmlDoc - rootElement is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& rootElem = *static_cast<XmlElement*>(rootElement);

	// Ability
	XmlElement* parentElem = doc.NewElement("LaserAbilityDef");
	parentElem->SetAttribute("name", m_name.ToCStr());
	rootElem.InsertEndChild(parentElem);

	m_targetingDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_onHitDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_renderDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_chainDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_multishotDef.WriteToXmlDoc(xmlDoc, parentElem);
}



//----------------------------------------------------------------------------------------------------------------------
void LaserAbilityDef::SwirlInto(AbilityDef& other) const
{
	other.ReceiveSwirl(*this);
}



//----------------------------------------------------------------------------------------------------------------------
bool LaserAbilityDef::ReceiveSwirl(ProjectileHitAbilityDef const& other)
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool LaserAbilityDef::ReceiveSwirl(AoEHitAbilityDef const& other)
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool LaserAbilityDef::ReceiveSwirl(PassiveAoEAbilityDef const& other)
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool LaserAbilityDef::ReceiveSwirl(LaserAbilityDef const& other)
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool LaserAbilityDef::ReceiveSwirl(AdjacentHitAbilityDef const& other)
{
	return false;
}
