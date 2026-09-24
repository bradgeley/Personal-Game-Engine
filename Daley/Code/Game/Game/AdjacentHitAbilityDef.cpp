// Bradley Christensen - 2022-2026
#include "AdjacentHitAbilityDef.h"
#include "Ability.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AdjacentHitAbilityDef::AdjacentHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    if (XmlElement const* cooldownElem = elem.FirstChildElement("Cooldown"))
    {
        m_cooldownDef = AbilityCooldownComponentDef(cooldownElem);
    }

    // targeting component is implicit, no data in it anyway

	if (XmlElement const* scalingElem = elem.FirstChildElement("Scaling"))
	{
		m_scaling = AbilityScalingComponentDef(scalingElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AdjacentHitAbilityDef::MakeAbilityInstance() const
{
	return new AdjacentHitAbility(*this);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* AdjacentHitAbilityDef::Copy() const
{
    return new AdjacentHitAbilityDef(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbilityDef::WriteToXmlDoc(void* xmlDoc, void* rootElement)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "AdjacentHitAbilityDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(rootElement != nullptr, "AdjacentHitAbilityDef::WriteToXmlDoc - rootElement is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& rootElem = *static_cast<XmlElement*>(rootElement);

	// Ability
	XmlElement* parentElem = doc.NewElement("AdjacentHitAbilityDef");
	parentElem->SetAttribute("name", m_name.ToCStr());
	rootElem.InsertEndChild(parentElem);

	m_cooldownDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_scaling.WriteToXmlDoc(xmlDoc, parentElem);
}



//----------------------------------------------------------------------------------------------------------------------
void AdjacentHitAbilityDef::SwirlInto(AbilityDef& other) const
{
	other.ReceiveSwirl(*this);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdjacentHitAbilityDef::ReceiveSwirl(ProjectileHitAbilityDef const&)
{
	// Empty, adjacent hit abilities don't receive swirls yet

	// Todo: If swirling vanilla into coffee, add a damage buff to nearby towers in addition to the haste,
	// same with other damage types

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AdjacentHitAbilityDef::ReceiveSwirl(AoEHitAbilityDef const& other)
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AdjacentHitAbilityDef::ReceiveSwirl(PassiveAoEAbilityDef const& other)
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AdjacentHitAbilityDef::ReceiveSwirl(LaserAbilityDef const& other)
{
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AdjacentHitAbilityDef::ReceiveSwirl(AdjacentHitAbilityDef const& other)
{
	return false;
}
