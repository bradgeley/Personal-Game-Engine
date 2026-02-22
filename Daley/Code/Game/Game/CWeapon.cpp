// Bradley Christensen - 2022-2026
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



//----------------------------------------------------------------------------------------------------------------------
CWeapon::CWeapon(CWeapon const& copyFrom)
{
	m_weapons.reserve(copyFrom.m_weapons.size());

	for (auto& weapon : copyFrom.m_weapons)
	{
		m_weapons.push_back(weapon->DeepCopy());
	}
}



//----------------------------------------------------------------------------------------------------------------------
CWeapon::CWeapon(CWeapon&& moveFrom) noexcept : m_weapons(std::move(moveFrom.m_weapons))
{
	moveFrom.m_weapons.clear();
}



//----------------------------------------------------------------------------------------------------------------------
CWeapon& CWeapon::operator=(CWeapon const& copyFrom)
{
	if (this == &copyFrom)
	{
		return *this;
	}

	for (Weapon* weapon : m_weapons)
	{
		delete weapon;
	}
	m_weapons.clear();

	m_weapons.reserve(copyFrom.m_weapons.size());

	for (auto& weapon : copyFrom.m_weapons)
	{
		m_weapons.push_back(weapon->DeepCopy());
	}

	return *this;
}



//----------------------------------------------------------------------------------------------------------------------
CWeapon& CWeapon::operator=(CWeapon&& moveFrom) noexcept
{
	if (this == &moveFrom)
	{
		return *this;
	}

	for (Weapon* weapon : m_weapons)
	{
		delete weapon;
	}
	m_weapons.clear();

	m_weapons = std::move(moveFrom.m_weapons);
	moveFrom.m_weapons.clear();

	return *this;
}
