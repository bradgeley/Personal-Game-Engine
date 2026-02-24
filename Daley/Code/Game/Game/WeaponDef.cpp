// Bradley Christensen - 2022-2026
#include "WeaponDef.h"
#include "EntityDef.h"
#include "Weapon.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
const char* s_weaponDefsFilePath = "Data/Definitions/WeaponDefs.xml";
std::vector<WeaponDef*> WeaponDef::s_weaponDefs;



//----------------------------------------------------------------------------------------------------------------------
void WeaponDef::LoadFromXML()
{
    XmlDocument doc;
    doc.LoadFile(s_weaponDefsFilePath);
    auto root = doc.RootElement();
    if (!root)
    {
		DevConsoleUtils::LogError("WeaponDef::LoadFromXml - Could not load file: %s", s_weaponDefsFilePath);
        return;
    }

    // Projectile weapons
    XmlElement* weaponDefElem = root->FirstChildElement("ProjectileHitWeaponDef");
    while (weaponDefElem)
    {
        Name name = XmlUtils::ParseXmlAttribute(*weaponDefElem, "name", Name::Invalid);
        if (GetWeaponDefID(name) != -1)
        {
			DevConsoleUtils::LogError("Duplicate Weapon Def: %s", name.ToCStr());
        }

        // Emplace new definition using the constructor that takes an Xml Element
		ProjectileHitWeaponDef* newDef = new ProjectileHitWeaponDef(weaponDefElem);
        s_weaponDefs.push_back(newDef);

        weaponDefElem = weaponDefElem->NextSiblingElement("ProjectileHitWeaponDef");
    }
}



//----------------------------------------------------------------------------------------------------------------------
void WeaponDef::Shutdown()
{
    for (WeaponDef* def : s_weaponDefs)
    {
        delete def;
    }
    s_weaponDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
WeaponDef const* WeaponDef::GetWeaponDef(uint8_t id)
{
    size_t index = static_cast<size_t>(id);
    return s_weaponDefs[index];
}



//----------------------------------------------------------------------------------------------------------------------
WeaponDef const* WeaponDef::GetWeaponDef(Name name)
{
    for (size_t i = 0; i < s_weaponDefs.size(); i++)
    {
        WeaponDef const* def = s_weaponDefs[i];
        if (def->m_name == name)
        {
            return def;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
int WeaponDef::GetWeaponDefID(Name name)
{
    for (size_t i = 0; i < s_weaponDefs.size(); i++)
    {
        WeaponDef const* def = s_weaponDefs[i];
        if (def->m_name == name)
        {
            return (int) i;
        }
    }
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
ProjectileHitWeaponDef::ProjectileHitWeaponDef(void const* xmlElement)
{
	// e.g. <ProjectileHitWeaponDef name = "VanillaWeapon" projectileDef = "VanillaProj" baseAttacksPerSecond = "1" baseDamage = "10">< / ProjectileHitWeaponDef>
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
Weapon* ProjectileHitWeaponDef::MakeBaseWeapon() const
{
	ProjectileHitWeapon* weapon = new ProjectileHitWeapon(*this);
    return weapon;
}
