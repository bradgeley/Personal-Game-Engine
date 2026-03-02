// Bradley Christensen - 2022-2026
#include "CAbility.h"
#include "AbilityDef.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CAbility::~CAbility()
{
	for (Ability* ability : m_abilitys)
	{
		delete ability;
	}
	m_abilitys.clear();
}



//----------------------------------------------------------------------------------------------------------------------
CAbility::CAbility(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	Name abilityDefName = XmlUtils::ParseXmlAttribute(elem, "name", Name::Invalid);
	AbilityDef const* abilityDef = AbilityDef::GetAbilityDef(abilityDefName);
	if (abilityDef)
	{
		Ability* ability = abilityDef->MakeAbilityInstance();
		m_abilitys.push_back(ability);
	}
	else 
	{
		DevConsoleUtils::LogError("CAbility constructor - Invalid ability def name: %s", abilityDefName.ToCStr());
	}
}



//----------------------------------------------------------------------------------------------------------------------
CAbility::CAbility(CAbility const& copyFrom)
{
	m_abilitys.reserve(copyFrom.m_abilitys.size());

	for (auto& ability : copyFrom.m_abilitys)
	{
		m_abilitys.push_back(ability->DeepCopy());
	}
}



//----------------------------------------------------------------------------------------------------------------------
CAbility::CAbility(CAbility&& moveFrom) noexcept : m_abilitys(std::move(moveFrom.m_abilitys))
{
	moveFrom.m_abilitys.clear();
}



//----------------------------------------------------------------------------------------------------------------------
CAbility& CAbility::operator=(CAbility const& copyFrom)
{
	if (this == &copyFrom)
	{
		return *this;
	}

	for (Ability* ability : m_abilitys)
	{
		delete ability;
	}
	m_abilitys.clear();

	m_abilitys.reserve(copyFrom.m_abilitys.size());

	for (auto& ability : copyFrom.m_abilitys)
	{
		m_abilitys.push_back(ability->DeepCopy());
	}

	return *this;
}



//----------------------------------------------------------------------------------------------------------------------
CAbility& CAbility::operator=(CAbility&& moveFrom) noexcept
{
	if (this == &moveFrom)
	{
		return *this;
	}

	for (Ability* ability : m_abilitys)
	{
		delete ability;
	}
	m_abilitys.clear();

	m_abilitys = std::move(moveFrom.m_abilitys);
	moveFrom.m_abilitys.clear();

	return *this;
}
