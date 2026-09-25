// Bradley Christensen - 2022-2026
#include "AbilityAttributes.h"
#include "EntityDebugContext.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AbilityAttributes::AbilityAttributes()
{
	m_values.fill(0.f);
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAttributes::AbilityAttributes(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	m_values[(int) EAbilityAttribute::Physical] = XmlUtils::ParseXmlAttribute(elem, "physical", 0.f);
	m_values[(int) EAbilityAttribute::Burn] = XmlUtils::ParseXmlAttribute(elem, "burn", 0.f);
	m_values[(int) EAbilityAttribute::Poison] = XmlUtils::ParseXmlAttribute(elem, "poison", 0.f);
	m_values[(int) EAbilityAttribute::SlowDuration] = XmlUtils::ParseXmlAttribute(elem, "slow", 0.f);
	m_values[(int) EAbilityAttribute::HasteDuration] = XmlUtils::ParseXmlAttribute(elem, "haste", 0.f);
	m_values[(int) EAbilityAttribute::AttackSpeed_Multi] = XmlUtils::ParseXmlAttribute(elem, "attackSpeed", 0.f);
	m_values[(int) EAbilityAttribute::Range_Multi] = XmlUtils::ParseXmlAttribute(elem, "range", 0.f);
	m_values[(int) EAbilityAttribute::AreaOfEffect_Multi] = XmlUtils::ParseXmlAttribute(elem, "aoe", 0.f);
	m_values[(int) EAbilityAttribute::AreaDamage_Mulit] = XmlUtils::ParseXmlAttribute(elem, "areaDamage", 0.f);
	m_values[(int) EAbilityAttribute::ProjectileSpeed_Multi] = XmlUtils::ParseXmlAttribute(elem, "projectileSpeed", 0.f);
	m_values[(int) EAbilityAttribute::CritChance_Add] = XmlUtils::ParseXmlAttribute(elem, "critChance", 0.f);
	m_values[(int) EAbilityAttribute::CritMulti_Multi] = XmlUtils::ParseXmlAttribute(elem, "critMulti", 0.f);
	m_values[(int) EAbilityAttribute::ChainCount_Add] = XmlUtils::ParseXmlAttribute(elem, "numChains", 0.f);
	m_values[(int) EAbilityAttribute::ChainChance_Add] = XmlUtils::ParseXmlAttribute(elem, "chainChance", 0.f);
	m_values[(int) EAbilityAttribute::ChainDistance_Multi] = XmlUtils::ParseXmlAttribute(elem, "chainDistance", 0.f);
	m_values[(int) EAbilityAttribute::MultishotCount_Add] = XmlUtils::ParseXmlAttribute(elem, "multishotCount", 0.f);
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityAttributes::GetValue(EAbilityAttribute attribute) const
{
	return m_values[(int) attribute];
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAttributes::AppendDebugString(EntityDebugContext& debugContext) const
{
	auto const& attributeNames = GetAttributeNames();

	debugContext.m_debugString += StringUtils::StringF("---Ability Attributes---\n");

	for (int i = 0; i < (int) EAbilityAttribute::Count; i++)
	{
		if (m_values[i] == 0.f)
		{
			continue;
		}
		debugContext.m_debugString += attributeNames[i].ToString() + ": " + std::to_string(m_values[i]) + "\n";
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAttributes::operator+=(AbilityAttributes const& other)
{
	for (int i = 0; i < (int) EAbilityAttribute::Count; i++)
	{
		m_values[i] += other.m_values[i];
	}
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAttributes AbilityAttributes::operator+(AbilityAttributes const& other) const
{
	AbilityAttributes result = *this;
	result += other;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAttributes AbilityAttributes::operator*(float value) const
{
	AbilityAttributes result = *this;
	for (int i = 0; i < (int) EAbilityAttribute::Count; i++)
	{
		result.m_values[i] *= value;
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::array<Name, (int) EAbilityAttribute::Count> const& AbilityAttributes::GetAttributeNames()
{
	static std::array<Name, (int) EAbilityAttribute::Count> s_names = 
	{
		Name("Physical"),
		Name("Burn"),
		Name("Poison"),
		Name("Slow Duration"),
		Name("Haste Duration"),
		Name("Attack Speed"),
		Name("Range"),
		Name("Area of Effect"),
		Name("Area Damage"),
		Name("Projectile Speed"),
		Name("Crit Chance"),
		Name("Crit Multi"),
		Name("Chain Count"),
		Name("Chain Chance"),
		Name("Chain Distance"),
		Name("Multishot Count")
	};
	return s_names;
}
