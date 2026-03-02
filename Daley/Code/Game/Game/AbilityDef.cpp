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
//  <ProjectileHitAbilityDef name="VanillaAttack" projectileDef="VanillaProj" projSpeed="100">
//      <Cooldown cooldown = "0.1">< / Cooldown>
//      <Targeting maxRange = "15">< / Targeting>
//      <Crit critChance = "0.1">< / Crit>
//      <OnHit>
//          <Damage minDamage = "10" maxDamage = "15">< / Damage>
//      </OnHit>
//  </ProjectileHitAbilityDef>
//  
//  <ProjectileHitAbilityDef name = "ChocolateAttack" projectileDef = "ChocolateProj" projSpeed = "50">
//  <Cooldown cooldown = "2">< / Cooldown>
//  <Targeting maxRange = "12">< / Targeting>
//  <Crit critChance = "0.05">< / Crit>
//  <OnHit>
//  <Damage minDamage = "25" maxDamage = "50">< / Damage>
//  <AoEHit radius = "2">
//  <Damage damage = "50">< / Damage>
//  < / AoEHit>
//  < / OnHit>
//  < / ProjectileHitAbilityDef>
//  
//  <ProjectileHitAbilityDef name = "PistachioAttack" projectileDef = "PistachioProj" projSpeed = "50">
//  <Cooldown cooldown = "2">< / Cooldown>
//  <Targeting maxRange = "12">< / Targeting>
//  <OnHit>
//  <AoEHit radius = "2">
//  <Poison poison = "5">< / Poison>
//  < / AoEHit>
//  <AoEEffect duration = "2" radius = "2">
//  <Poison poison = "5">< / Poison>
//  < / AoEEffect>
//  < / OnHit>
//  < / ProjectileHitAbilityDef>
//  
//  <ProjectileHitAbilityDef name = "StrawberryAttack" projectileDef = "StrawberryProj" projSpeed = "100">
//  <Cooldown cooldown = "0.5">< / Cooldown>
//  <Targeting maxRange = "6">< / Targeting>
//  <OnHit>
//  <Burn burn = "10">< / Burn>
//  <AoEEffect duration = "0.5" radius = "0.5">
//  <Burn burn = "10">< / Burn>
//  < / AoEEffect>
//  < / OnHit>
//  < / ProjectileHitAbilityDef>
//
ProjectileHitAbilityDef::ProjectileHitAbilityDef(void const* xmlElement)
{
	// e.g. <ProjectileHitAbilityDef name = "VanillaAbility" projectileDef = "VanillaProj" baseAttacksPerSecond = "1" baseDamage = "10">< / ProjectileHitAbilityDef>
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_name                      = XmlUtils::ParseXmlAttribute(elem, "name", m_projectileDefName);
	m_projectileDefName			= XmlUtils::ParseXmlAttribute(elem, "projectileDef", m_projectileDefName);
	m_baseDamage				= XmlUtils::ParseXmlAttribute(elem, "baseDamage", m_baseDamage);
	m_baseAttacksPerSecond		= XmlUtils::ParseXmlAttribute(elem, "baseAttacksPerSecond", m_baseAttacksPerSecond);
	m_baseProjSpeedUnitsPerSec	= XmlUtils::ParseXmlAttribute(elem, "baseProjSpeed", m_baseProjSpeedUnitsPerSec);
    m_baseMinRange              = XmlUtils::ParseXmlAttribute(elem, "baseMinRange", m_baseMinRange);
	m_baseMaxRange              = XmlUtils::ParseXmlAttribute(elem, "baseMaxRange", m_baseMaxRange);
	m_splashDamage              = XmlUtils::ParseXmlAttribute(elem, "splashDamage", m_splashDamage);
	m_splashRadius              = XmlUtils::ParseXmlAttribute(elem, "splashRadius", m_splashRadius);
}



//----------------------------------------------------------------------------------------------------------------------
Ability* ProjectileHitAbilityDef::MakeAbilityInstance() const
{
	ProjectileHitAbility* ability = new ProjectileHitAbility(*this);
    return ability;
}
