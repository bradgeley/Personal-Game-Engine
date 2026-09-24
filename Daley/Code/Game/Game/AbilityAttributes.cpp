// Bradley Christensen - 2022-2026
#include "AbilityAttributes.h"
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
	m_values[(int) EAbilityAttribute::AttackSpeed] = XmlUtils::ParseXmlAttribute(elem, "attackSpeed", 0.f);
	m_values[(int) EAbilityAttribute::Range] = XmlUtils::ParseXmlAttribute(elem, "range", 0.f);
	m_values[(int) EAbilityAttribute::AreaOfEffect] = XmlUtils::ParseXmlAttribute(elem, "aoe", 0.f);
	m_values[(int) EAbilityAttribute::AreaDamage] = XmlUtils::ParseXmlAttribute(elem, "areaDamage", 0.f);
	m_values[(int) EAbilityAttribute::ProjectileSpeed] = XmlUtils::ParseXmlAttribute(elem, "projectileSpeed", 0.f);
	m_values[(int) EAbilityAttribute::CritChance] = XmlUtils::ParseXmlAttribute(elem, "critChance", 0.f);
	m_values[(int) EAbilityAttribute::CritMulti] = XmlUtils::ParseXmlAttribute(elem, "critMulti", 0.f);
	m_values[(int) EAbilityAttribute::ChainCount] = XmlUtils::ParseXmlAttribute(elem, "numChains", 0.f);
	m_values[(int) EAbilityAttribute::ChainChance] = XmlUtils::ParseXmlAttribute(elem, "chainChance", 0.f);
	m_values[(int) EAbilityAttribute::ChainDistance] = XmlUtils::ParseXmlAttribute(elem, "chainDistance", 0.f);
	m_values[(int) EAbilityAttribute::MultishotCount] = XmlUtils::ParseXmlAttribute(elem, "multishotCount", 0.f);
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
