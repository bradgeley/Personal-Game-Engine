// Bradley Christensen - 2022-2026
#include "AoEHitAbilityDef.h"
#include "Ability.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AoEHitAbilityDef::AoEHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

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
    if (XmlElement const* aoeHitElem = elem.FirstChildElement("AoEHit"))
    {
        m_aoeHitDef = AbilityAoEHitComponentDef(aoeHitElem);
    }
    if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
    {
        m_aoeEffectDef = AbilityAoEEffectComponentDef(aoeEffectElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AoEHitAbilityDef::MakeAbilityInstance() const
{
    AoEHitAbility* ability = new AoEHitAbility(*this);
    return ability;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* AoEHitAbilityDef::Copy() const
{
    return new AoEHitAbilityDef(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbilityDef::WriteToXmlDoc(void* xmlDoc, void* rootElement)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "AoEHitAbilityDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(rootElement != nullptr, "AoEHitAbilityDef::WriteToXmlDoc - rootElement is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& rootElem = *static_cast<XmlElement*>(rootElement);

	// Ability
	XmlElement* parentElem = doc.NewElement("AoEHitAbilityDef");
	parentElem->SetAttribute("name", m_name.ToCStr());
	rootElem.InsertEndChild(parentElem);

	m_cooldownDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_targetingDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_critDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_aoeHitDef.WriteToXmlDoc(xmlDoc, parentElem);
	m_aoeEffectDef.WriteToXmlDoc(xmlDoc, parentElem);
}



//----------------------------------------------------------------------------------------------------------------------
void AoEHitAbilityDef::SwirlInto(AbilityDef& other) const
{
	other.ReceiveSwirl(*this);
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(ProjectileHitAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(AoEHitAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(PassiveAoEAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(LaserAbilityDef const& other)
{
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool AoEHitAbilityDef::ReceiveSwirl(AdjacentHitAbilityDef const& other)
{
    return false;
}
