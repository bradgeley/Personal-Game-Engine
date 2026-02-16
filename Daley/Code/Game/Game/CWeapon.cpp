// Bradley Christensen - 2022-2025
#include "CWeapon.h"
#include "WeaponDef.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CWeapon::~CWeapon()
{
	for (Weapon* weapon : m_weapons)
	{
		delete weapon;
	}
	m_weapons.clear();
}



//----------------------------------------------------------------------------------------------------------------------
CWeapon::CWeapon(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	Name weaponDefName = XmlUtils::ParseXmlAttribute(elem, "name", Name::Invalid);
	WeaponDef const* weaponDef = WeaponDef::GetWeaponDef(weaponDefName);
	if (weaponDef)
	{
		Weapon* weapon = weaponDef->MakeBaseWeapon();
		m_weapons.push_back(weapon);
	}
	else 
	{
		DevConsoleUtils::LogError("CWeapon constructor - Invalid weapon def name: %s", weaponDefName.ToCStr());
	}
}