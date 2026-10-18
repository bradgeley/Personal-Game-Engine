// Bradley Christensen - 2022-2026
#include "AbilityAttributes.h"
#include "EntityDebugContext.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AbilityAttributes::AbilityAttributes(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	auto const& attributeNames = GetAttributeNames();

	for (int i = 0; i < (int) AbilityAttribute::Count; ++i)
	{
		AbilityAttribute attribute = (AbilityAttribute) i;
		float value = XmlUtils::ParseXmlAttribute(elem, attributeNames[i].ToCStr(), 0.f);
		if (value != 0.f)
		{
			SetValue(attribute, value);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
float AbilityAttributes::GetValue(AbilityAttribute attribute) const
{
	for (int i = 0; i < m_count; ++i)
	{
		if (m_keys[i] == attribute)
		{
			return m_values[i];
		}
	}
	return 0.f;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAttributes::SetValue(AbilityAttribute attribute, float value)
{
	for (int i = 0; i < m_count; ++i)
	{
		if (m_keys[i] == attribute)
		{
			m_values[i] = value;
			return;
		}
	}

	ASSERT_OR_DIE(m_count < s_maxAttributes, "AbilityAttributes::SetValue - exceeded s_maxAttributes, raise the cap or reduce the number of attributes on this ability.");
	m_keys[m_count] = attribute;
	m_values[m_count] = value;
	m_count++;
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAttributes::AddValue(AbilityAttribute attribute, float delta)
{
	SetValue(attribute, GetValue(attribute) + delta);
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAttributes::AppendDebugString(EntityDebugContext& debugContext) const
{
	if (m_count == 0)
	{
		return;
	}

	auto const& attributeNames = GetAttributeNames();

	debugContext.m_debugString += StringUtils::StringF("---Ability Attributes---\n");

	for (int i = 0; i < m_count; ++i)
	{
		int attributeIndex = (int) m_keys[i];
		debugContext.m_debugString += attributeNames[attributeIndex].ToString() + ": " + std::to_string(m_values[i]) + "\n";
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AbilityAttributes::operator+=(AbilityAttributes const& other)
{
	for (int i = 0; i < other.m_count; ++i)
	{
		AddValue(other.m_keys[i], other.m_values[i]);
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
	for (int i = 0; i < result.m_count; ++i)
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
		Name("Vulnerability"),
		Name("Slow"),
		Name("Haste"),

		Name("PhysicalMulti"),
		Name("BurnMulti"),
		Name("PoisonMulti"),
		Name("VulnerabilityMulti"),
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
		Name("MultishotCount"),

		Name("PhysicalResistance"),
		Name("BurnResistance"),
		Name("PoisonResistance"),
		Name("SlowResistance"),
		Name("HealthMulti"),
		Name("MovementSpeedMulti"),
	};
	return s_names;
}



//----------------------------------------------------------------------------------------------------------------------
AttributeDisplayInfo const& AbilityAttributes::GetAttributeDisplayInfo(AbilityAttribute attribute)
{
	static std::array<AttributeDisplayInfo, (int) AbilityAttribute::Count> s_infos =
	{
		AttributeDisplayInfo{Name("Physical"),					AttributeDisplayType::FlatValue,			"+%d"},
		AttributeDisplayInfo{Name("Burn"),						AttributeDisplayType::FlatValue,			"+%d"},
		AttributeDisplayInfo{Name("Poison"),					AttributeDisplayType::FlatValue,			"+%d"},
		AttributeDisplayInfo{Name("Vulnerability"),				AttributeDisplayType::FlatValue,			"+%d"},
		AttributeDisplayInfo{Name("Slow Duration"),				AttributeDisplayType::Seconds,			   "+%ds"},
		AttributeDisplayInfo{Name("Haste Duration"),			AttributeDisplayType::Seconds,			   "+%ds"},

		AttributeDisplayInfo{Name("Physical Multiplier"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Burn Multiplier"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Poison Multiplier"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Vulnerability Multiplier"),	AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Slow Multiplier"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Haste Multiplier"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},

		AttributeDisplayInfo{Name("Attack Speed"),				AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Range"),						AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Area of Effect"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Area Damage"),				AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Projectile Speed"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Crit Chance"),				AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Crit Multiplier"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Chain Count"),				AttributeDisplayType::FlatSigned,			"+%d"},
		AttributeDisplayInfo{Name("Chain Chance"),				AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Chain Distance"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Multishot Count"),			AttributeDisplayType::FlatSigned,			"+%d"},

		AttributeDisplayInfo{Name("Physical Resistance"),		AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Burn Resistance"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Poison Resistance"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Slow Resistance"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Health Multiplier"),			AttributeDisplayType::PercentSigned,	"+%.0f%%"},
		AttributeDisplayInfo{Name("Movement Speed Multiplier"),	AttributeDisplayType::PercentSigned,	"+%.0f%%"}
	};


	return s_infos[(int) attribute];
}



//----------------------------------------------------------------------------------------------------------------------
std::string AbilityAttributes::FormatAttribute(AbilityAttribute attr, float value)
{
	AttributeDisplayInfo const& info = GetAttributeDisplayInfo(attr);

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
			return StringUtils::StringF("%.1fs", value);
		default:
			return "Invalid display type";
	}
}