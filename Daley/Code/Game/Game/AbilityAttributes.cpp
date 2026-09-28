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

	auto const& attributeNames = GetAttributeNames();

	m_values[(int) AbilityAttribute::Physical] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Physical].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::Burn] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Burn].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::Poison] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Poison].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::SlowDuration] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::SlowDuration].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::HasteDuration] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::HasteDuration].ToCStr(), 0.f);

	m_values[(int) AbilityAttribute::Physical_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Physical_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::Burn_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Burn_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::Poison_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Poison_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::Slow_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Slow_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::Haste_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Haste_Multi].ToCStr(), 0.f);

	m_values[(int) AbilityAttribute::AttackSpeed_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::AttackSpeed_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::Range_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::Range_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::AreaOfEffect_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::AreaOfEffect_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::AreaDamage_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::AreaDamage_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::ProjectileSpeed_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::ProjectileSpeed_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::CritChance_Add] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::CritChance_Add].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::CritMulti_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::CritMulti_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::ChainCount_Add] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::ChainCount_Add].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::ChainChance_Add] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::ChainChance_Add].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::ChainDistance_Multi] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::ChainDistance_Multi].ToCStr(), 0.f);
	m_values[(int) AbilityAttribute::MultishotCount_Add] = XmlUtils::ParseXmlAttribute(elem, attributeNames[(int) AbilityAttribute::MultishotCount_Add].ToCStr(), 0.f);
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityAttributes::GetValue(AbilityAttribute attribute) const
{
	return m_values[(int) attribute];
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAttributes::AppendDebugString(EntityDebugContext& debugContext) const
{
	auto const& attributeNames = GetAttributeNames();

	debugContext.m_debugString += StringUtils::StringF("---Ability Attributes---\n");

	for (int i = 0; i < (int) AbilityAttribute::Count; i++)
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
	for (int i = 0; i < (int) AbilityAttribute::Count; i++)
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
	for (int i = 0; i < (int) AbilityAttribute::Count; i++)
	{
		result.m_values[i] *= value;
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::array<Name, (int) AbilityAttribute::Count> const& AbilityAttributes::GetAttributeNames()
{
	static std::array<Name, (int) AbilityAttribute::Count> s_names = 
	{
		Name("Physical"),
		Name("Burn"),
		Name("Poison"),
		Name("Slow"),
		Name("Haste"),

		Name("PhysicalMulti"),
		Name("BurnMulti"),
		Name("PoisonMulti"),
		Name("SlowMulti"),
		Name("HasteMulti"),

		Name("AttackSpeed"),
		Name("Range"),
		Name("AreaofEffect"),
		Name("AreaDamage"),
		Name("ProjectileSpeed"),
		Name("CritChance"),
		Name("CritMulti"),
		Name("ChainCount"),
		Name("ChainChance"),
		Name("ChainDistance"),
		Name("MultishotCount")
	};
	return s_names;
}



//----------------------------------------------------------------------------------------------------------------------
AbilityAttributeDisplayInfo const& AbilityAttributes::GetAttributeDisplayInfo(AbilityAttribute attribute)
{
	static std::array<AbilityAttributeDisplayInfo, (int) AbilityAttribute::Count> s_infos =
	{
		AbilityAttributeDisplayInfo{Name("Physical"),				AttributeDisplayType::FlatValue,			"+%d"},
		AbilityAttributeDisplayInfo{Name("Burn"),					AttributeDisplayType::FlatValue,			"+%d"},
		AbilityAttributeDisplayInfo{Name("Poison"),					AttributeDisplayType::FlatValue,			"+%d"},
		AbilityAttributeDisplayInfo{Name("Slow Duration"),			AttributeDisplayType::Seconds,			   "+%ds"},
		AbilityAttributeDisplayInfo{Name("Haste Duration"),			AttributeDisplayType::Seconds,			   "+%ds"},
		AbilityAttributeDisplayInfo{Name("Physical Multiplier"),	AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Burn Multiplier"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Poison Multiplier"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Slow Multiplier"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Haste Multiplier"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},

		AbilityAttributeDisplayInfo{Name("Attack Speed"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Range"),					AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Area of Effect"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Area Damage"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Projectile Speed"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Crit Chance"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Crit Multiplier"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Chain Count"),			AttributeDisplayType::FlatSigned,			"+%d"},
		AbilityAttributeDisplayInfo{Name("Chain Chance"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Chain Distance"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AbilityAttributeDisplayInfo{Name("Multishot Count"),		AttributeDisplayType::FlatSigned,			"+%d"}
	};


	return s_infos[(int)attribute];
}



//----------------------------------------------------------------------------------------------------------------------
std::string AbilityAttributes::FormatAttribute(AbilityAttribute attr, float value)
{
	AbilityAttributeDisplayInfo const& info = GetAttributeDisplayInfo(attr);

	switch (info.m_displayType)
	{
		case AttributeDisplayType::FlatValue:
			return StringUtils::StringF("%.0f", value);

		case AttributeDisplayType::FlatSigned:
			return StringUtils::StringF("%+.0f", value);

		case AttributeDisplayType::Percent:
			return StringUtils::StringF("%.0f%%", value * 100.f);

		case AttributeDisplayType::PercentSigned:
			return StringUtils::StringF("%+.0f%%", value * 100.f);

		case AttributeDisplayType::MultiplierPercent:
			return StringUtils::StringF("%+.0f%%", (value - 1.f) * 100.f);

		case AttributeDisplayType::Seconds:
			return StringUtils::StringF("%.*fs", value);
		default:
			return "Invalid display type";
	}
}
