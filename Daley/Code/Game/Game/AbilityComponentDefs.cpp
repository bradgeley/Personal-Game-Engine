// Bradley Christensen - 2022-2026
#include "AbilityComponentDefs.h"
#include "GameCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
const char* s_defaultAoEEffectName = "AoEEffect";



//----------------------------------------------------------------------------------------------------------------------
AbilityCooldownComponentDef::AbilityCooldownComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_cooldownSeconds = XmlUtils::ParseXmlAttribute(elem, "cooldown", m_cooldownSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityCooldownComponentDef defaultValues;
	if (m_cooldownSeconds == defaultValues.m_cooldownSeconds)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityCooldownComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityCooldownComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* cooldownElem = doc.NewElement("Cooldown");
	cooldownElem->SetAttribute("cooldown", m_cooldownSeconds);
	abilityElem.InsertEndChild(cooldownElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityTargetingComponentDef::AbilityTargetingComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_maxRange = XmlUtils::ParseXmlAttribute(elem, "maxRange", m_maxRange);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityTargetingComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityTargetingComponentDef defaultValues;
	if (m_maxRange == defaultValues.m_maxRange)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityTargetingComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityTargetingComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* targetingElem = doc.NewElement("Targeting");
	if (m_maxRange != defaultValues.m_maxRange)
	{
		targetingElem->SetAttribute("maxRange", m_maxRange);
	}
	abilityElem.InsertEndChild(targetingElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCritComponentDef::AbilityCritComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_critChance = XmlUtils::ParseXmlAttribute(elem, "critChance", m_critChance);
	m_critMulti = XmlUtils::ParseXmlAttribute(elem, "critMulti", m_critMulti);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCritComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityCritComponentDef defaultValues;
	if (m_critChance == defaultValues.m_critChance && m_critMulti == defaultValues.m_critMulti)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityCritComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityCritComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* critElem = doc.NewElement("Crit");
	if (m_critChance != defaultValues.m_critChance)
	{
		critElem->SetAttribute("critChance", m_critChance);
	}
	if (m_critMulti != defaultValues.m_critMulti)
	{
		critElem->SetAttribute("critMulti", m_critMulti);
	}
	abilityElem.InsertEndChild(critElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityScalingComponentDef::AbilityScalingComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_physical = XmlUtils::ParseXmlAttribute(elem, "physical", m_physical);
	m_burn = XmlUtils::ParseXmlAttribute(elem, "burn", m_burn);
	m_poison = XmlUtils::ParseXmlAttribute(elem, "poison", m_poison);
	m_slow = XmlUtils::ParseXmlAttribute(elem, "slow", m_slow);
	m_haste = XmlUtils::ParseXmlAttribute(elem, "haste", m_haste);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityScalingComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityScalingComponentDef defaultValues;
	if (m_physical == defaultValues.m_physical && m_burn == defaultValues.m_burn && m_poison == defaultValues.m_poison && m_slow == defaultValues.m_slow && m_haste == defaultValues.m_haste)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityScalingComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityScalingComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* scalingElem = doc.NewElement("Scaling");
	if (m_physical != defaultValues.m_physical)
	{
		scalingElem->SetAttribute("physical", m_physical);
	}
	if (m_burn != defaultValues.m_burn)
	{
		scalingElem->SetAttribute("burn", m_burn);
	}
	if (m_poison != defaultValues.m_poison)
	{
		scalingElem->SetAttribute("poison", m_poison);
	}
	if (m_slow != defaultValues.m_slow)
	{
		scalingElem->SetAttribute("slow", m_slow);
	}
	if (m_haste != defaultValues.m_haste)
	{
		scalingElem->SetAttribute("haste", m_haste);
	}
	abilityElem.InsertEndChild(scalingElem);
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
void AbilityChainComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	AbilityChainComponentDef defaultValues;
	if (m_chainChance == defaultValues.m_chainChance && m_chainDistance == defaultValues.m_chainDistance && m_chainPayloadMulti == defaultValues.m_chainPayloadMulti && m_maxChains == defaultValues.m_maxChains)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityChainComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityChainComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* chainElem = doc.NewElement("Chain");
	if (m_chainChance != defaultValues.m_chainChance)
	{
		chainElem->SetAttribute("chainChance", m_chainChance);
	}
	if (m_chainDistance != defaultValues.m_chainDistance)
	{
		chainElem->SetAttribute("chainDistance", m_chainDistance);
	}
	if (m_chainPayloadMulti != defaultValues.m_chainPayloadMulti)
	{
		chainElem->SetAttribute("chainPayloadMulti", m_chainPayloadMulti);
	}
	if (m_maxChains != defaultValues.m_maxChains)
	{
		chainElem->SetAttribute("maxChains", m_maxChains);
	}
	abilityElem.InsertEndChild(chainElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityMultishotComponentDef::AbilityMultishotComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_additionalTargets = XmlUtils::ParseXmlAttribute(elem, "additionalTargets", m_additionalTargets);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityMultishotComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityMultishotComponentDef defaultValues;
	if (m_additionalTargets == defaultValues.m_additionalTargets)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityMultishotComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityMultishotComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* multishotElem = doc.NewElement("Multishot");
	if (m_additionalTargets != defaultValues.m_additionalTargets)
	{
		multishotElem->SetAttribute("additionalTargets", m_additionalTargets);
	}
	abilityElem.InsertEndChild(multishotElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityRenderComponentDef::AbilityRenderComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_tint = XmlUtils::ParseXmlAttribute(elem, "tint", m_tint);
	m_depth = XmlUtils::ParseXmlAttribute(elem, "depth", StaticGameSettings::s_defaultCollisionEffectDepth);
	m_renderDuration = XmlUtils::ParseXmlAttribute(elem, "duration", m_renderDuration);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityRenderComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityRenderComponentDef defaultValues;
	if (m_tint == defaultValues.m_tint && m_depth == defaultValues.m_depth && m_renderDuration == defaultValues.m_renderDuration)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityRenderComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityRenderComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* renderElem = doc.NewElement("Render");
	if (m_tint != defaultValues.m_tint)
	{
		renderElem->SetAttribute("tint", StringUtils::Rgba8ToString(m_tint).c_str());
	}
	if (m_depth != defaultValues.m_depth)
	{
		renderElem->SetAttribute("depth", m_depth);
	}
	if (m_renderDuration != defaultValues.m_renderDuration)
	{
		renderElem->SetAttribute("duration", m_renderDuration);
	}
	abilityElem.InsertEndChild(renderElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEHitComponentDef::AbilityAoEHitComponentDef(void const* xmlElement)
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
void AbilityAoEHitComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityAoEHitComponentDef defaultValues;

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityAoEHitComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityAoEHitComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* aoeHitElem = doc.NewElement("AoEHit");
	if (m_radius != defaultValues.m_radius)
	{
		aoeHitElem->SetAttribute("radius", m_radius);
	}
	abilityElem.InsertEndChild(aoeHitElem);

	m_scaling.WriteToXmlDoc(xmlDoc, aoeHitElem);
	m_renderDef.WriteToXmlDoc(xmlDoc, aoeHitElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEEffectComponentDef::AbilityAoEEffectComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_aoeEffectDefName = XmlUtils::ParseXmlAttribute(elem, "name", Name(s_defaultAoEEffectName));
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
void AbilityAoEEffectComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityAoEEffectComponentDef defaultValues;
	if (m_aoeEffectDefName == Name::Invalid || (m_aoeEffectDefName == defaultValues.m_aoeEffectDefName 
		&& m_radius == defaultValues.m_radius && m_durationSeconds == defaultValues.m_durationSeconds))
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityAoEEffectComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityAoEEffectComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* aoeEffectElem = doc.NewElement("AoEEffect");
	aoeEffectElem->SetAttribute("name", m_aoeEffectDefName.ToCStr());
	if (m_radius != defaultValues.m_radius)
	{
		aoeEffectElem->SetAttribute("radius", m_radius);
	}
	if (m_durationSeconds != defaultValues.m_durationSeconds)
	{
		aoeEffectElem->SetAttribute("duration", m_durationSeconds);
	}
	abilityElem.InsertEndChild(aoeEffectElem);

	m_scaling.WriteToXmlDoc(xmlDoc, aoeEffectElem);
	m_renderDef.WriteToXmlDoc(xmlDoc, aoeEffectElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityOnHitComponentDef::AbilityOnHitComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	if (XmlElement const* scaling = elem.FirstChildElement("Scaling"))
	{
		m_scaling = AbilityScalingComponentDef(scaling);
	}
	if (XmlElement const* aoeHitElem = elem.FirstChildElement("AoEHit"))
	{
		m_aoeHitOnHit = AbilityAoEHitComponentDef(aoeHitElem);
	}
	if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
	{
		m_aoeEffectOnHit = AbilityAoEEffectComponentDef(aoeEffectElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityOnHitComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityOnHitComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityOnHitComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* onHitElem = doc.NewElement("OnHit");
	abilityElem.InsertEndChild(onHitElem);

	m_scaling.WriteToXmlDoc(xmlDoc, onHitElem);
	m_aoeHitOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
	m_aoeEffectOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
}