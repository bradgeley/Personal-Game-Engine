// Bradley Christensen - 2022-2026
#include "AbilityDef.h"
#include "EntityDef.h"
#include "Ability.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
const char* s_abilityDefsFilePath = "Data/Definitions/AbilityDefs.xml";
std::vector<AbilityDef*> AbilityDef::s_abilityDefs;



//----------------------------------------------------------------------------------------------------------------------
void AbilityDef::LoadFromXML()
{
    XmlDocument doc;
    doc.LoadFile(s_abilityDefsFilePath);
    auto root = doc.RootElement();
    if (!root)
    {
		DevConsoleUtils::LogError("AbilityDef::LoadFromXml - Could not load file: %s", s_abilityDefsFilePath);
        return;
    }

    // Projectile abilitys
    XmlElement* abilityDefElem = root->FirstChildElement("ProjectileHitAbilityDef");
    while (abilityDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*abilityDefElem, "name", Name::Invalid);
        if (GetAbilityDefID(name) != -1)
        {
			DevConsoleUtils::LogError("Duplicate Ability Def: %s", name.ToCStr());
        }

        // Emplace new definition using the constructor that takes an Xml Element
		ProjectileHitAbilityDef* newDef = new ProjectileHitAbilityDef(abilityDefElem);
        s_abilityDefs.push_back(newDef);

        abilityDefElem = abilityDefElem->NextSiblingElement("ProjectileHitAbilityDef");
    }
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
ProjectileHitAbilityDef::ProjectileHitAbilityDef(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_name                      = XmlUtils::ParseXmlAttribute(elem, "name", m_projectileDefName);
	m_projectileDefName			= XmlUtils::ParseXmlAttribute(elem, "projectileDef", m_projectileDefName);
	m_projSpeed	                = XmlUtils::ParseXmlAttribute(elem, "projSpeed", m_projSpeed);

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
}



//----------------------------------------------------------------------------------------------------------------------
Ability* ProjectileHitAbilityDef::MakeAbilityInstance() const
{
	ProjectileHitAbility* ability = new ProjectileHitAbility(*this);
    return ability;
}



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
	m_minRange = XmlUtils::ParseXmlAttribute(elem, "minRange", m_minRange);
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
AbilityBurnComponentDef::AbilityBurnComponentDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_burn = XmlUtils::ParseXmlAttribute(elem, "burn", m_burn);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityPoisonComponentDef::AbilityPoisonComponentDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
	m_poison = XmlUtils::ParseXmlAttribute(elem, "poison", m_poison);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoeHitComponentDef::AbilityAoeHitComponentDef(void const* xmlElement)
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
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAoeEffectComponentDef::AbilityAoeEffectComponentDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_radius = XmlUtils::ParseXmlAttribute(elem, "radius", m_radius);
    m_durationSeconds = XmlUtils::ParseXmlAttribute(elem, "duration", m_durationSeconds);

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
    if (XmlElement const* aoeEffectElem = elem.FirstChildElement("AoeEffect"))
    {
        m_aoeEffectOnHit.emplace(aoeEffectElem);
    }
}
