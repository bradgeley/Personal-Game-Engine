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
	m_minRange = XmlUtils::ParseXmlAttribute(elem, "minRange", m_minRange);
	m_maxRange = XmlUtils::ParseXmlAttribute(elem, "maxRange", m_maxRange);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityTargetingComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityTargetingComponentDef defaultValues;
	if (m_minRange == defaultValues.m_minRange && m_maxRange == defaultValues.m_maxRange)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityTargetingComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityTargetingComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* targetingElem = doc.NewElement("Targeting");
	if (m_minRange != defaultValues.m_minRange)
	{
		targetingElem->SetAttribute("minRange", m_minRange);
	}
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
AbilityDamageComponentDef::AbilityDamageComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	float damage = XmlUtils::ParseXmlAttribute(elem, "damage", 0.f);

	if (damage == 0.f)
	{
		m_minDamage = XmlUtils::ParseXmlAttribute(elem, "minDamage", m_minDamage);
		m_maxDamage = XmlUtils::ParseXmlAttribute(elem, "maxDamage", m_maxDamage);
	}
	else
	{
		m_minDamage = damage;
		m_maxDamage = damage;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityDamageComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityDamageComponentDef defaultValues;
	if (m_minDamage == defaultValues.m_minDamage && m_maxDamage == defaultValues.m_maxDamage)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityDamageComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityDamageComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* damageElem = doc.NewElement("Damage");
	if (m_minDamage != defaultValues.m_minDamage)
	{
		damageElem->SetAttribute("minDamage", m_minDamage);
	}
	if (m_maxDamage != defaultValues.m_maxDamage)
	{
		damageElem->SetAttribute("maxDamage", m_maxDamage);
	}
	abilityElem.InsertEndChild(damageElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityBurnComponentDef::AbilityBurnComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_burn = XmlUtils::ParseXmlAttribute(elem, "burn", m_burn);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityBurnComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityBurnComponentDef defaultValues;
	if (m_burn == defaultValues.m_burn)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityBurnComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityBurnComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* burnElem = doc.NewElement("Burn");
	burnElem->SetAttribute("burn", m_burn);
	abilityElem.InsertEndChild(burnElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityPoisonComponentDef::AbilityPoisonComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_poison = XmlUtils::ParseXmlAttribute(elem, "poison", m_poison);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityPoisonComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityPoisonComponentDef defaultValues;
	if (m_poison == defaultValues.m_poison)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityPoisonComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityPoisonComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* poisonElem = doc.NewElement("Poison");
	poisonElem->SetAttribute("poison", m_poison);
	abilityElem.InsertEndChild(poisonElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilitySlowComponentDef::AbilitySlowComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_duration = XmlUtils::ParseXmlAttribute(elem, "duration", m_duration);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilitySlowComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilitySlowComponentDef defaultValues;
	if (m_duration == defaultValues.m_duration)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilitySlowComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilitySlowComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* slowElem = doc.NewElement("Slow");
	slowElem->SetAttribute("duration", m_duration);
	abilityElem.InsertEndChild(slowElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityHasteComponentDef::AbilityHasteComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_duration = XmlUtils::ParseXmlAttribute(elem, "duration", m_duration);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityHasteComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
	static AbilityHasteComponentDef defaultValues;
	if (m_duration == defaultValues.m_duration)
	{
		return;
	}

	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityHasteComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityHasteComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* hasteElem = doc.NewElement("Haste");
	hasteElem->SetAttribute("duration", m_duration);
	abilityElem.InsertEndChild(hasteElem);
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

	if (XmlElement const* damageElem = elem.FirstChildElement("Damage"))
	{
		m_damageOnHit = AbilityDamageComponentDef(damageElem);
	}
	if (XmlElement const* poisonElem = elem.FirstChildElement("Poison"))
	{
		m_poisonOnHit = AbilityPoisonComponentDef(poisonElem);
	}
	if (XmlElement const* burnElem = elem.FirstChildElement("Burn"))
	{
		m_burnOnHit = AbilityBurnComponentDef(burnElem);
	}
	if (XmlElement const* slowElem = elem.FirstChildElement("Slow"))
	{
		m_slowOnHit = AbilitySlowComponentDef(slowElem);
	}
	if (XmlElement const* hasteElem = elem.FirstChildElement("Haste"))
	{
		m_hasteOnHit = AbilityHasteComponentDef(hasteElem);
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

	m_damageOnHit.WriteToXmlDoc(xmlDoc, aoeHitElem);
	m_poisonOnHit.WriteToXmlDoc(xmlDoc, aoeHitElem);
	m_burnOnHit.WriteToXmlDoc(xmlDoc, aoeHitElem);
	m_slowOnHit.WriteToXmlDoc(xmlDoc, aoeHitElem);
	m_hasteOnHit.WriteToXmlDoc(xmlDoc, aoeHitElem);
	m_renderDef.WriteToXmlDoc(xmlDoc, aoeHitElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEEffectComponentDef::AbilityAoEEffectComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_aoeEffectDefName = XmlUtils::ParseXmlAttribute(elem, "name", Name(s_defaultAoEEffectName));
	m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);
	m_durationSeconds = XmlUtils::ParseXmlAttribute(elem, "duration", m_durationSeconds);

	if (XmlElement const* damageElem = elem.FirstChildElement("Damage"))
	{
		m_damagePerSecond = AbilityDamageComponentDef(damageElem);
	}
	if (XmlElement const* poisonElem = elem.FirstChildElement("Poison"))
	{
		m_poisonPerSecond = AbilityPoisonComponentDef(poisonElem);
	}
	if (XmlElement const* burnElem = elem.FirstChildElement("Burn"))
	{
		m_burnPerSecond = AbilityBurnComponentDef(burnElem);
	}
	if (XmlElement const* slowElem = elem.FirstChildElement("Slow"))
	{
		m_slowPerSecond = AbilitySlowComponentDef(slowElem);
	}
	if (XmlElement const* hasteElem = elem.FirstChildElement("Haste"))
	{
		m_hastePerSecond = AbilityHasteComponentDef(hasteElem);
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

	m_damagePerSecond.WriteToXmlDoc(xmlDoc, aoeEffectElem);
	m_poisonPerSecond.WriteToXmlDoc(xmlDoc, aoeEffectElem);
	m_burnPerSecond.WriteToXmlDoc(xmlDoc, aoeEffectElem);
	m_slowPerSecond.WriteToXmlDoc(xmlDoc, aoeEffectElem);
	m_hastePerSecond.WriteToXmlDoc(xmlDoc, aoeEffectElem);
	m_renderDef.WriteToXmlDoc(xmlDoc, aoeEffectElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityOnHitComponentDef::AbilityOnHitComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	if (XmlElement const* damageElem = elem.FirstChildElement("Damage"))
	{
		m_damageOnHit = AbilityDamageComponentDef(damageElem);
	}
	if (XmlElement const* poisonElem = elem.FirstChildElement("Poison"))
	{
		m_poisonOnHit = AbilityPoisonComponentDef(poisonElem);
	}
	if (XmlElement const* burnElem = elem.FirstChildElement("Burn"))
	{
		m_burnOnHit = AbilityBurnComponentDef(burnElem);
	}
	if (XmlElement const* aoeHitElem = elem.FirstChildElement("AoEHit"))
	{
		m_aoeHitOnHit = AbilityAoEHitComponentDef(aoeHitElem);
	}
	if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
	{
		m_aoeEffectOnHit = AbilityAoEEffectComponentDef(aoeEffectElem);
	}
	if (XmlElement const* slowElem = elem.FirstChildElement("Slow"))
	{
		m_slowOnHit = AbilitySlowComponentDef(slowElem);
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

	m_damageOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
	m_poisonOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
	m_burnOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
	m_slowOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
	m_aoeHitOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
	m_aoeEffectOnHit.WriteToXmlDoc(xmlDoc, onHitElem);
}