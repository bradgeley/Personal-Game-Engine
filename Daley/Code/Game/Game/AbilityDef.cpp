// Bradley Christensen - 2022-2026
#include "AbilityDef.h"
#include "Ability.h"
#include "EntityDef.h"
#include "GameCommon.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
const char* s_defaultAoEEffectName = "AoEEffect";
std::vector<AbilityDef*> AbilityDef::s_abilityDefs;



//----------------------------------------------------------------------------------------------------------------------
void AbilityDef::LoadFromXML(Name filepath)
{
    XmlDocument doc;
    doc.LoadFile(filepath.ToCStr());
    auto root = doc.RootElement();
    if (!root)
    {
		DevConsoleUtils::LogError("AbilityDef::LoadFromXml - Could not load file: %s", filepath.ToCStr());
        return;
    }

    // Projectile abilities
    XmlElement* abilityDefElem = root->FirstChildElement("ProjectileHitAbilityDef");
    while (abilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*abilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
			DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

		ProjectileHitAbilityDef* newDef = new ProjectileHitAbilityDef(abilityDefElem);
        s_abilityDefs.push_back(newDef);

        abilityDefElem = abilityDefElem->NextSiblingElement("ProjectileHitAbilityDef");
    }

    // AoE Hit Abilities
	XmlElement* aoeHitAbilityDefElem = root->FirstChildElement("AoEHitAbilityDef");
    while (aoeHitAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*aoeHitAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        AoEHitAbilityDef* newDef = new AoEHitAbilityDef(aoeHitAbilityDefElem);
        s_abilityDefs.push_back(newDef);

        aoeHitAbilityDefElem = aoeHitAbilityDefElem->NextSiblingElement("AoEHitAbilityDef");
    }


	// Passive AoE Abilities
	XmlElement* passiveAoEAbilityDefElem = root->FirstChildElement("PassiveAoEAbilityDef");
    while (passiveAoEAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*passiveAoEAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        PassiveAoEAbilityDef* newDef = new PassiveAoEAbilityDef(passiveAoEAbilityDefElem);
        s_abilityDefs.push_back(newDef);
        passiveAoEAbilityDefElem = passiveAoEAbilityDefElem->NextSiblingElement("PassiveAoEAbilityDef");
	}

    // Adjacent Hit Abilities
	XmlElement* adjacentHitAbilityDefElem = root->FirstChildElement("AdjacentHitAbilityDef");
    while (adjacentHitAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*adjacentHitAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        AdjacentHitAbilityDef* newDef = new AdjacentHitAbilityDef(adjacentHitAbilityDefElem);
        s_abilityDefs.push_back(newDef);
        adjacentHitAbilityDefElem = adjacentHitAbilityDefElem->NextSiblingElement("AdjacentHitAbilityDef");
	}

    // Laser Abilities
	XmlElement* laserAbilityDefElem = root->FirstChildElement("LaserAbilityDef");
    while (laserAbilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*laserAbilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
            DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        LaserAbilityDef* newDef = new LaserAbilityDef(laserAbilityDefElem);
        s_abilityDefs.push_back(newDef);
        laserAbilityDefElem = laserAbilityDefElem->NextSiblingElement("LaserAbilityDef");
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityDef::SaveToXML(Name filepath)
{
    XmlDocument doc;

	XmlElement* rootElem = doc.NewElement("AbilityDefs");
	doc.InsertFirstChild(rootElem);

	for (AbilityDef* def : s_abilityDefs)
	{
		def->WriteToXmlDoc(&doc, rootElem);
	}

	doc.SaveFile(filepath.ToCStr());
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityDef::Shutdown()
{
    for (AbilityDef* def : s_abilityDefs)
    {
        delete def;
    }
    s_abilityDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef const* AbilityDef::GetAbilityDef(uint8_t id)
{
    size_t index = static_cast<size_t>(id);
    return s_abilityDefs[index];
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef const* AbilityDef::GetAbilityDef(Name name)
{
    for (size_t i = 0; i < s_abilityDefs.size(); i++)
    {
        AbilityDef const* def = s_abilityDefs[i];
        if (def->m_name == name)
        {
            return def;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
int AbilityDef::GetAbilityDefID(Name name)
{
    for (size_t i = 0; i < s_abilityDefs.size(); i++)
    {
        AbilityDef const* def = s_abilityDefs[i];
        if (def->m_name == name)
        {
            return (int) i;
        }
    }
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityCooldownComponentDef::AbilityCooldownComponentDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_cooldownSeconds = XmlUtils::ParseXmlAttribute(elem, "cooldown", m_cooldownSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityCooldownComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
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
    ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityTargetingComponentDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(parentElem != nullptr, "AbilityTargetingComponentDef::WriteToXmlDoc - parentElem is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* targetingElem = doc.NewElement("Targeting");
    if (m_minRange != 0.f)
    {
        targetingElem->SetAttribute("minRange", m_minRange);
    }
	if (m_maxRange != 0.f)
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
    ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityCritComponentDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(parentElem != nullptr, "AbilityCritComponentDef::WriteToXmlDoc - parentElem is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* critElem = doc.NewElement("Crit");
    if (m_critChance != 0.f)
    {
        critElem->SetAttribute("critChance", m_critChance);
    }
	if (m_critMulti != 0.f)
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
    ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityDamageComponentDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(parentElem != nullptr, "AbilityDamageComponentDef::WriteToXmlDoc - parentElem is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* damageElem = doc.NewElement("Damage");
    if (m_minDamage != 0.f)
    {
        damageElem->SetAttribute("minDamage", m_minDamage);
    }
	if (m_maxDamage != 0.f)
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
	ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityChainComponentDef::WriteToXmlDoc - xmlDoc is null.");
	ASSERT_OR_DIE(parentElem != nullptr, "AbilityChainComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

    XmlElement* chainElem = doc.NewElement("Chain");
    if (m_chainChance != 1.f)
    {
		chainElem->SetAttribute("chainChance", m_chainChance);
    }
    if (m_chainDistance != 3.f)
    {
		chainElem->SetAttribute("chainDistance", m_chainDistance);
    }
	if (m_chainPayloadMulti != 1.f)
	{
		chainElem->SetAttribute("chainPayloadMulti", m_chainPayloadMulti);
	}
	if (m_maxChains != 0)
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
    ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityMultishotComponentDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(parentElem != nullptr, "AbilityMultishotComponentDef::WriteToXmlDoc - parentElem is null.");

	XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
	XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

    XmlElement* multishotElem = doc.NewElement("Multishot");
    if (m_additionalTargets != 0)
    {
		multishotElem->SetAttribute("additionalTargets", m_additionalTargets);
	}
	abilityElem.InsertEndChild(multishotElem);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEHitComponentDef::AbilityAoEHitComponentDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);
    if (XmlElement const* damageElem = elem.FirstChildElement("Damage"))
    {
        m_damageOnHit.emplace(damageElem);
    }
    if (XmlElement const* poisonElem = elem.FirstChildElement("Poison"))
    {
        m_poisonOnHit.emplace(poisonElem);
    }
    if (XmlElement const* burnElem = elem.FirstChildElement("Burn"))
    {
        m_burnOnHit.emplace(burnElem);
	}
    if (XmlElement const* slowElem = elem.FirstChildElement("Slow"))
    {
        m_slowOnHit.emplace(slowElem);
	}
	if (XmlElement const* hasteElem = elem.FirstChildElement("Haste"))
	{
		m_hasteOnHit.emplace(hasteElem);
	}
	if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
	{
		m_renderDef.emplace(renderElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEHitComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityAoEHitComponentDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(parentElem != nullptr, "AbilityAoEHitComponentDef::WriteToXmlDoc - parentElem is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* aoeHitElem = doc.NewElement("AoEHit");
	aoeHitElem->SetAttribute("radius", m_radius);
	abilityElem.InsertEndChild(aoeHitElem);

	if (m_damageOnHit.has_value())
	{
		m_damageOnHit->WriteToXmlDoc(xmlDoc, aoeHitElem);
	}
	if (m_poisonOnHit.has_value())
	{
		m_poisonOnHit->WriteToXmlDoc(xmlDoc, aoeHitElem);
	}
	if (m_burnOnHit.has_value())
	{
		m_burnOnHit->WriteToXmlDoc(xmlDoc, aoeHitElem);
	}
	if (m_slowOnHit.has_value())
	{
		m_slowOnHit->WriteToXmlDoc(xmlDoc, aoeHitElem);
	}
	if (m_hasteOnHit.has_value())
	{
		m_hasteOnHit->WriteToXmlDoc(xmlDoc, aoeHitElem);
	}   
	if (m_renderDef.has_value())
	{
		m_renderDef->WriteToXmlDoc(xmlDoc, aoeHitElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoEEffectComponentDef::AbilityAoEEffectComponentDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);
    m_durationSeconds = XmlUtils::ParseXmlAttribute(elem, "duration", m_durationSeconds);
    m_aoeEffectDefName = XmlUtils::ParseXmlAttribute(elem, "name", Name(s_defaultAoEEffectName));

    if (XmlElement const* damageElem = elem.FirstChildElement("Damage"))
    {
        m_damagePerSecond.emplace(damageElem);
    }
    if (XmlElement const* poisonElem = elem.FirstChildElement("Poison"))
    {
        m_poisonPerSecond.emplace(poisonElem);
    }
    if (XmlElement const* burnElem = elem.FirstChildElement("Burn"))
    {
        m_burnPerSecond.emplace(burnElem);
	}
    if (XmlElement const* slowElem = elem.FirstChildElement("Slow"))
    {
        m_slowPerSecond.emplace(slowElem);
	}
	if (XmlElement const* hasteElem = elem.FirstChildElement("Haste"))
	{
		m_hastePerSecond.emplace(hasteElem);
	}
    if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
    {
        m_renderDef.emplace(renderElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAoEEffectComponentDef::WriteToXmlDoc(void* xmlDoc, void* parentElem)
{
    ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityAoEEffectComponentDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(parentElem != nullptr, "AbilityAoEEffectComponentDef::WriteToXmlDoc - parentElem is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

	XmlElement* aoeEffectElem = doc.NewElement("AoEEffect");
	aoeEffectElem->SetAttribute("name", m_aoeEffectDefName.ToCStr());
	aoeEffectElem->SetAttribute("radius", m_radius);
	aoeEffectElem->SetAttribute("duration", m_durationSeconds);
    abilityElem.InsertEndChild(aoeEffectElem);

	if (m_damagePerSecond.has_value())
	{
		m_damagePerSecond->WriteToXmlDoc(xmlDoc, aoeEffectElem);
	}
	if (m_poisonPerSecond.has_value())
	{
		m_poisonPerSecond->WriteToXmlDoc(xmlDoc, aoeEffectElem);
	}
	if (m_burnPerSecond.has_value())
	{
		m_burnPerSecond->WriteToXmlDoc(xmlDoc, aoeEffectElem);
	}
	if (m_slowPerSecond.has_value())
	{
		m_slowPerSecond->WriteToXmlDoc(xmlDoc, aoeEffectElem);
	}
	if (m_hastePerSecond.has_value())
	{
		m_hastePerSecond->WriteToXmlDoc(xmlDoc, aoeEffectElem);
	}
	if (m_renderDef.has_value())
	{
		m_renderDef->WriteToXmlDoc(xmlDoc, aoeEffectElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityOnHitComponentDef::AbilityOnHitComponentDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    if (XmlElement const* damageElem = elem.FirstChildElement("Damage"))
    {
        m_damageOnHit.emplace(damageElem);
	}
    if (XmlElement const* poisonElem = elem.FirstChildElement("Poison"))
    {
        m_poisonOnHit.emplace(poisonElem);
    }
    if (XmlElement const* burnElem = elem.FirstChildElement("Burn"))
    {
        m_burnOnHit.emplace(burnElem);
	}
    if (XmlElement const* aoeHitElem = elem.FirstChildElement("AoEHit"))
    {
        m_aoeHitOnHit.emplace(aoeHitElem);
	}
    if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
    {
        m_aoeEffectOnHit.emplace(aoeEffectElem);
    }
    if (XmlElement const* slowElem = elem.FirstChildElement("Slow"))
    {
        m_slowOnHit.emplace(slowElem);
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

    if (m_damageOnHit.has_value())
    {
        m_damageOnHit->WriteToXmlDoc(xmlDoc, onHitElem);
    }
    if (m_poisonOnHit.has_value())
    {
        m_poisonOnHit->WriteToXmlDoc(xmlDoc, onHitElem);
    }
    if (m_burnOnHit.has_value())
    {
        m_burnOnHit->WriteToXmlDoc(xmlDoc, onHitElem);
    }
    if (m_slowOnHit.has_value())
    {
        m_slowOnHit->WriteToXmlDoc(xmlDoc, onHitElem);
    }
	if (m_aoeHitOnHit.has_value())
	{
		m_aoeHitOnHit->WriteToXmlDoc(xmlDoc, onHitElem);
	}
	if (m_aoeEffectOnHit.has_value())
	{
		m_aoeEffectOnHit->WriteToXmlDoc(xmlDoc, onHitElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitAbilityDef::ProjectileHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_projectileDefName = XmlUtils::ParseXmlAttribute(elem, "projectileDef", m_projectileDefName);
    m_projSpeed = XmlUtils::ParseXmlAttribute(elem, "projSpeed", m_projSpeed);

    if (XmlElement const* cooldownElem = elem.FirstChildElement("Cooldown"))
    {
        m_cooldownDef.emplace(cooldownElem);
    }
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef.emplace(targetingElem);
    }
    if (XmlElement const* critElem = elem.FirstChildElement("Crit"))
    {
        m_critDef.emplace(critElem);
    }
    if (XmlElement const* onHitElem = elem.FirstChildElement("OnHit"))
    {
        m_onHitDef.emplace(onHitElem);
    }
    if (XmlElement const* chainElem = elem.FirstChildElement("Chain"))
    {
        m_chainDef.emplace(chainElem);
    }
    if (XmlElement const* multishotElem = elem.FirstChildElement("Multishot"))
    {
        m_multishotDef.emplace(multishotElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* ProjectileHitAbilityDef::MakeAbilityInstance() const
{
    ProjectileHitAbility* ability = new ProjectileHitAbility(*this);
    return ability;
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

	if (m_cooldownDef.has_value())
	{
		m_cooldownDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_targetingDef.has_value())
	{
		m_targetingDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_critDef.has_value())
	{
		m_critDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_chainDef.has_value())
	{
		m_chainDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_multishotDef.has_value())
	{
		m_multishotDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_onHitDef.has_value())
	{
		m_onHitDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AoEHitAbilityDef::AoEHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    if (XmlElement const* cooldownElem = elem.FirstChildElement("Cooldown"))
    {
        m_cooldownDef.emplace(cooldownElem);
    }
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef.emplace(targetingElem);
    }
    if (XmlElement const* critElem = elem.FirstChildElement("Crit"))
    {
        m_critDef.emplace(critElem);
    }
    if (XmlElement const* aoeHitElem = elem.FirstChildElement("AoEHit"))
    {
        m_aoeHitDef.emplace(aoeHitElem);
    }
    if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
    {
        m_aoeEffectDef.emplace(aoeEffectElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityDef::AbilityDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_name = XmlUtils::ParseXmlAttribute(elem, "name", m_name);
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AoEHitAbilityDef::MakeAbilityInstance() const
{
    AoEHitAbility* ability = new AoEHitAbility(*this);
    return ability;
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

	if (m_cooldownDef.has_value())
	{
		m_cooldownDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_targetingDef.has_value())
	{
		m_targetingDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_critDef.has_value())
	{
		m_critDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_aoeHitDef.has_value())
	{
		m_aoeHitDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_aoeEffectDef.has_value())
	{
		m_aoeEffectDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
PassiveAoEAbilityDef::PassiveAoEAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef.emplace(targetingElem);
	}
    if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoEEffect"))
    {
        m_aoeEffectDef.emplace(aoeEffectElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* PassiveAoEAbilityDef::MakeAbilityInstance() const
{
    PassiveAoEAbility* ability = new PassiveAoEAbility(*this);
	return ability;
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

	if (m_targetingDef.has_value())
	{
		m_targetingDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_aoeEffectDef.has_value())
	{
		m_aoeEffectDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AdjacentHitAbilityDef::AdjacentHitAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

    if (XmlElement const* cooldownElem = elem.FirstChildElement("Cooldown"))
    {
        m_cooldownDef.emplace(cooldownElem);
    }

    // targeting component is implicit, no data in it anyway

	if (XmlElement const* hasteElem = elem.FirstChildElement("Haste"))
	{
		m_hasteOnHit.emplace(hasteElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
Ability* AdjacentHitAbilityDef::MakeAbilityInstance() const
{
	return new AdjacentHitAbility(*this);
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

	if (m_cooldownDef.has_value())
	{
		m_cooldownDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_hasteOnHit.has_value())
	{
		m_hasteOnHit->WriteToXmlDoc(xmlDoc, parentElem);
	}
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
    ASSERT_OR_DIE(xmlDoc != nullptr, "AbilityRenderComponentDef::WriteToXmlDoc - xmlDoc is null.");
    ASSERT_OR_DIE(parentElem != nullptr, "AbilityRenderComponentDef::WriteToXmlDoc - parentElem is null.");

    XmlDocument& doc = *static_cast<XmlDocument*>(xmlDoc);
    XmlElement& abilityElem = *static_cast<XmlElement*>(parentElem);

    XmlElement* renderElem = doc.NewElement("Render");
    renderElem->SetAttribute("tint", StringUtils::Rgba8ToString(m_tint).c_str());
    renderElem->SetAttribute("depth", m_depth);
    renderElem->SetAttribute("duration", m_renderDuration);
    abilityElem.InsertEndChild(renderElem);
}



//----------------------------------------------------------------------------------------------------------------------
LaserAbilityDef::LaserAbilityDef(void const* xmlElement) : AbilityDef(xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    if (XmlElement const* targetingElem = elem.FirstChildElement("Targeting"))
    {
        m_targetingDef.emplace(targetingElem);
    }
    if (XmlElement const* onHitElem = elem.FirstChildElement("OnHit"))
    {
        m_onHitDef.emplace(onHitElem);
	}
    if (XmlElement const* renderElem = elem.FirstChildElement("Render"))
    {
        m_renderDef.emplace(renderElem);
	}
    if (XmlElement const* chainElem = elem.FirstChildElement("Chain"))
    {
        m_chainDef.emplace(chainElem);
    }
    if (XmlElement const* multishotElem = elem.FirstChildElement("Multishot"))
    {
        m_multishotDef.emplace(multishotElem);
    }
}



//----------------------------------------------------------------------------------------------------------------------
Ability* LaserAbilityDef::MakeAbilityInstance() const
{
    LaserAbility* ability = new LaserAbility(*this);
	return ability;
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

	if (m_targetingDef.has_value())
	{
		m_targetingDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_onHitDef.has_value())
	{
		m_onHitDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_renderDef.has_value())
	{
		m_renderDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_chainDef.has_value())
	{
		m_chainDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
	if (m_multishotDef.has_value())
	{
		m_multishotDef->WriteToXmlDoc(xmlDoc, parentElem);
	}
}
