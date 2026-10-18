// Bradley Christensen - 2022-2026
#include "AuraAbilityDef.h"
#include "Ability.h"
#include "AuraAbility.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AuraAbilityDef::AuraAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	Name typeStr = XmlUtils::ParseXmlAttribute(elem, "type", Name::Invalid);
	if (typeStr == "Protection")
	{
		m_auraType = AuraType::Protection;
	}
	else if (typeStr == "Haste")
	{
		m_auraType = AuraType::Haste;
	}
	else if (typeStr == "Slow")
	{
		m_auraType = AuraType::Slow;
	}
	else if (typeStr == "BurnSuppression")
	{
		m_auraType = AuraType::BurnSuppression;
	}
	else if (typeStr == "PoisonSuppression")
	{
		m_auraType = AuraType::PoisonSuppression;
	}
	else if (typeStr == "Healing")
	{
		m_auraType = AuraType::Healing;
	}

    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef = AbilityTargetingComponentDef(targetingElem);
	}
    if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
    {
        m_renderDef = AbilityRenderComponentDef(renderElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AuraAbilityDef::MakeAbilityInstance() const
{
    AuraAbility* ability = new AuraAbility(*this);
	return ability;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef* AuraAbilityDef::Copy() const
{
    return new AuraAbilityDef(*this);
}