// Bradley Christensen - 2022-2026
#include "CAbility.h"
#include "AbilityDef.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
CAbility::~CAbility()
{
	for (Ability* ability : m_abilities)
	{
		delete ability;
	}
	m_abilities.clear();
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
		m_abilities.push_back(ability);
	}
	else 
	{
		DevConsoleUtils::LogError("CAbility constructor - Invalid ability def name: %s", abilityDefName.ToCStr());
	}
}



//----------------------------------------------------------------------------------------------------------------------
CAbility::CAbility(CAbility const& copyFrom)
{
	m_abilities.reserve(copyFrom.m_abilities.size());

	for (auto& ability : copyFrom.m_abilities)
	{
		m_abilities.push_back(ability->DeepCopy());
	}
}



//----------------------------------------------------------------------------------------------------------------------
CAbility::CAbility(CAbility&& moveFrom) noexcept : m_abilities(std::move(moveFrom.m_abilities))
{
	moveFrom.m_abilities.clear();
}



//----------------------------------------------------------------------------------------------------------------------
CAbility& CAbility::operator=(CAbility const& copyFrom)
{
	if (this == &copyFrom)
	{
		return *this;
	}

	for (Ability* ability : m_abilities)
	{
		delete ability;
	}
	m_abilities.clear();

	m_abilities.reserve(copyFrom.m_abilities.size());

	for (auto& ability : copyFrom.m_abilities)
	{
		m_abilities.push_back(ability->DeepCopy());
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

	for (Ability* ability : m_abilities)
	{
		delete ability;
	}
	m_abilities.clear();

	m_abilities = std::move(moveFrom.m_abilities);
	moveFrom.m_abilities.clear();

	return *this;
}
