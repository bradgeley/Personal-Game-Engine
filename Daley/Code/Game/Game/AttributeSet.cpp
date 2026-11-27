// Bradley Christensen - 2022-2026
#include "AttributeSet.h"
#include "AttributeDef.h"
#include "EntityDebugContext.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/XmlUtils.h"



//----------------------------------------------------------------------------------------------------------------------
AttributeSet::AttributeSet()
{

}



//----------------------------------------------------------------------------------------------------------------------
AttributeSet::AttributeSet(void const* xmlElement)
{
	XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);

	for (XmlAttribute const* xmlAttr = elem.FirstAttribute(); xmlAttr != nullptr; xmlAttr = xmlAttr->Next())
	{
		Name const attrName(xmlAttr->Name());

		AttributeDef const* def = AttributeDef::GetAttributeDef(attrName); // Validate that the attribute exists
		ASSERT_OR_DIE(def != nullptr, StringUtils::StringF("AttributeSet - unknown attribute \"%s\" in XML", xmlAttr->Name()).c_str());

		AddValue(attrName, xmlAttr->FloatValue());
	}
}



//----------------------------------------------------------------------------------------------------------------------
float AttributeSet::GetValue(Name attribute, float defaultValue /*=0.f*/, bool errorIfNotFound /*=false*/) const
{
	for (int i = 0; i < m_count; ++i)
	{
		if (m_keys[i] == attribute)
		{
			return m_values[i];
		}
	}
	if (errorIfNotFound)
	{
		ERROR_AND_DIE(StringUtils::StringF("AttributeSet::GetValue - attribute not found in set: %s", attribute.ToString().c_str()));
	}
	return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
void AttributeSet::SetValue(Name attribute, float value)
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
void AttributeSet::AddValue(Name attribute, float delta)
{
	for (int i = 0; i < m_count; ++i)
	{
		if (m_keys[i] == attribute)
		{
			m_values[i] += delta;
			return;
		}
	}

	ASSERT_OR_DIE(m_count < s_maxAttributes, "AbilityAttributes::SetValue - exceeded s_maxAttributes, raise the cap or reduce the number of attributes on this ability.");
	m_keys[m_count] = attribute;
	m_values[m_count] = delta;
	m_count++;
}



//----------------------------------------------------------------------------------------------------------------------
void AttributeSet::Clear()
{
	std::fill(m_keys.begin(), m_keys.end(), Name::Invalid);
	std::fill(m_values.begin(), m_values.end(), 0.f);
	m_count = 0;
}



//----------------------------------------------------------------------------------------------------------------------
void AttributeSet::AppendDebugString(EntityDebugContext& debugContext) const
{
	if (m_count == 0)
	{
		return;
	}

	debugContext.m_debugString += StringUtils::StringF("---Ability Attributes---\n");

	for (int i = 0; i < m_count; ++i)
	{
		if (AttributeDef const* def = AttributeDef::GetAttributeDef(m_keys[i]))
		{
			std::string formattedValue = FormatAttribute(m_keys[i], m_values[i]);
			std::string attributeString = StringUtils::StringF("%s: %s\n", def->m_displayInfo.m_displayName.ToCStr(), formattedValue.c_str());
			debugContext.m_debugString += attributeString;
			continue;
		}

		debugContext.m_debugString += StringUtils::StringF("Invalid attribute: %s (%f)\n", m_keys[i].ToCStr(), m_values[i]);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AttributeSet::operator+=(AttributeSet const& other)
{
	for (int i = 0; i < other.m_count; ++i)
	{
		AddValue(other.m_keys[i], other.m_values[i]);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AttributeSet AttributeSet::operator+(AttributeSet const& other) const
{
	AttributeSet result = *this;
	result += other;
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
AttributeSet AttributeSet::operator*(float value) const
{
	AttributeSet result = *this;
	for (int i = 0; i < result.m_count; ++i)
	{
		result.m_values[i] *= value;
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::string AttributeSet::FormatAttribute(Name attr, float value)
{
	AttributeDef const* def = AttributeDef::GetAttributeDef(attr);
	if (!def)
	{
		return StringUtils::StringF("Invalid attribute: %s (%f)", attr.ToCStr(), value);
	}

	switch (def->m_displayInfo.m_displayType)
	{
		case AttributeDisplayType::Integer:
			return StringUtils::StringF("%.0f", value);

		case AttributeDisplayType::IntegerSigned:
			return StringUtils::StringF("%+.0f", value);

		case AttributeDisplayType::Decimal:
			return StringUtils::StringF("%.2f", value);

		case AttributeDisplayType::DecimalSigned:
			return StringUtils::StringF("%+.2f", value);

		case AttributeDisplayType::Percent:
			return StringUtils::StringF("%.0f%%", value * 100.f);

		case AttributeDisplayType::PercentSigned:
			return StringUtils::StringF("%+.0f%%", value * 100.f);

		case AttributeDisplayType::Seconds:
			return StringUtils::StringF("%.1fs", value);

		default:
			return "Invalid display type";
	}
}