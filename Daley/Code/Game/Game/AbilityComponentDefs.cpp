// Bradley Christensen - 2022-2026
#include "AbilityComponentDefs.h"
#include "GameCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
const char* s_defaultPuddleName = "Puddle";



//----------------------------------------------------------------------------------------------------------------------
AbilityCooldownComponentDef::AbilityCooldownComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_cooldownSeconds = XmlUtils::ParseXmlAttribute(elem, "cooldown", m_cooldownSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityTargetingComponentDef::AbilityTargetingComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_maxRange = XmlUtils::ParseXmlAttribute(elem, "maxRange", m_maxRange);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCritComponentDef::AbilityCritComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_critChance = XmlUtils::ParseXmlAttribute(elem, "critChance", m_critChance);
	m_critMulti = XmlUtils::ParseXmlAttribute(elem, "critMulti", m_critMulti);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityScalingComponentDef::AbilityScalingComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_physical = XmlUtils::ParseXmlAttribute(elem, "Physical", m_physical);
	m_burn = XmlUtils::ParseXmlAttribute(elem, "Burn", m_burn);
	m_poison = XmlUtils::ParseXmlAttribute(elem, "Poison", m_poison);
	m_vulnerability = XmlUtils::ParseXmlAttribute(elem, "Vulnerability", m_vulnerability);
	m_slow = XmlUtils::ParseXmlAttribute(elem, "Slow", m_slow);
	m_haste = XmlUtils::ParseXmlAttribute(elem, "Haste", m_haste);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityChainComponentDef::AbilityChainComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_chainChance = XmlUtils::ParseXmlAttribute(elem, "chainChance", m_chainChance);
	m_chainDistance = XmlUtils::ParseXmlAttribute(elem, "chainDistance", m_chainDistance);
	m_chainPayloadMulti = XmlUtils::ParseXmlAttribute(elem, "chainPayloadMulti", m_chainPayloadMulti);
	m_maxChains = XmlUtils::ParseXmlAttribute(elem, "maxChains", m_maxChains);

	ASSERT_OR_DIE(m_maxChains <= StaticGameSettings::s_maxChainTargets, StringUtils::StringF("AbilityChainComponentDef: m_maxChains (%d) exceeded StaticGameSettings::s_maxChainTargets (%d).", m_maxChains, StaticGameSettings::s_maxChainTargets));
}



//----------------------------------------------------------------------------------------------------------------------
AbilityMultishotComponentDef::AbilityMultishotComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_additionalTargets = XmlUtils::ParseXmlAttribute(elem, "additionalTargets", m_additionalTargets);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityRenderComponentDef::AbilityRenderComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_tint = XmlUtils::ParseXmlAttribute(elem, "tint", m_tint);
	m_depth = XmlUtils::ParseXmlAttribute(elem, "depth", StaticGameSettings::s_defaultPuddleRenderDepth);
	m_renderDuration = XmlUtils::ParseXmlAttribute(elem, "duration", m_renderDuration);
}



//----------------------------------------------------------------------------------------------------------------------
AbilitySplashComponentDef::AbilitySplashComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);

	if (XmlElement const* scaling = elem.FirstChildElement("Scaling"))
	{
		m_scaling = AbilityScalingComponentDef(scaling);
	}
	if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
	{
		m_renderDef = AbilityRenderComponentDef(renderElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityPuddleComponentDef::AbilityPuddleComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_puddleDefName = XmlUtils::ParseXmlAttribute(elem, "name", Name(s_defaultPuddleName));
	m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);
	m_durationSeconds = XmlUtils::ParseXmlAttribute(elem, "duration", m_durationSeconds);

	if (XmlElement const* scaling = elem.FirstChildElement("Scaling"))
	{
		m_scaling = AbilityScalingComponentDef(scaling);
	}
	if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
	{
		m_renderDef = AbilityRenderComponentDef(renderElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityOnHitComponentDef::AbilityOnHitComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	if (XmlElement const* scaling = elem.FirstChildElement("Scaling"))
	{
		m_scaling = AbilityScalingComponentDef(scaling);
	}
	if (XmlElement const* splashElem = elem.FirstChildElement("Splash"))
	{
		m_splashOnHit = AbilitySplashComponentDef(splashElem);
	}
	if (XmlElement const* puddleElem = elem.FirstChildElement("Puddle"))
	{
		m_puddleOnHit = AbilityPuddleComponentDef(puddleElem);
	}
}