// Bradley Christensen - 2022-2026
#include "PassiveAoEAbilityDef.h"
#include "Ability.h"
#include "PassiveAoEAbility.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
PassiveAoEAbilityDef::PassiveAoEAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef = AbilityTargetingComponentDef(targetingElem);
	}
    if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
    {
        m_aoeEffectDef = AbilityAoEEffectComponentDef(aoeEffectElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* PassiveAoEAbilityDef::MakeAbilityInstance() const
{
    PassiveAoEAbility* ability = new PassiveAoEAbility(*this);
	return ability;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* PassiveAoEAbilityDef::Copy() const
{
    return new PassiveAoEAbilityDef(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void PassiveAoEAbilityDef::WriteToXmlDoc(void* xmlDoc, void* rootElement)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "PassiveAoEAbilityDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(rootElement != nullptr, "PassiveAoEAbilityDef::WriteToXmlDoc - rootElement is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& rootElem = *static_cast<XmlElement*>(rootElement);

	// Ability
	XmlElement* parentElem = doc.NewElement("PassiveAoEAbilityDef");
	parentElem->SetAttribute("name", m_name.ToCStr());
	rootElem.InsertEndChild(parentElem);

	m_targetingDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_aoeEffectDef.WriteToXmlDoc(xmlDoc, parentElem);
}