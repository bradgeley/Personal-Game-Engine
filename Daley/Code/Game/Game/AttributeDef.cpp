// Bradley Christensen - 2022-2026
#include "AttributeDef.h"
#include "Engine/Core/XmlUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
std::vector<AttributeDef const*> AttributeDef::s_attributeDefs;



//----------------------------------------------------------------------------------------------------------------------
void AttributeDef::LoadFromXML(Name filepath)
{
    XmlDocument doc;
    doc.LoadFile(filepath.ToCStr());
    auto root = doc.RootElement();
    if (!root)
    {
		DevConsoleUtils::LogError("AttributeDef::LoadFromXml - Could not load file: %s", filepath.ToCStr());
        return;
    }

	XmlElement const* attributeElement = root->FirstChildElement("Attribute");

	while (attributeElement != nullptr)
	{
		AttributeDef* def = new AttributeDef(attributeElement);
		s_attributeDefs.push_back(def);
		attributeElement = attributeElement->NextSiblingElement("Attribute");
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AttributeDef::Shutdown()
{
    for (AttributeDef const* def : s_attributeDefs)
    {
        delete def;
    }
    s_attributeDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
AttributeDef const* AttributeDef::GetAttributeDef(uint8_t id)
{
    size_t index = static_cast<size_t>(id);
    return s_attributeDefs[index];
}



//----------------------------------------------------------------------------------------------------------------------
AttributeDef const* AttributeDef::GetAttributeDef(Name name)
{
    for (size_t i = 0; i < s_attributeDefs.size(); i++)
    {
        AttributeDef const* def = s_attributeDefs[i];
        if (def->m_name == name)
        {
            return def;
        }
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
int AttributeDef::GetAttributeDefID(Name name)
{
    for (size_t i = 0; i < s_attributeDefs.size(); i++)
    {
        AttributeDef const* def = s_attributeDefs[i];
        if (def->m_name == name)
        {
            return (int) i;
        }
    }
    return -1;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<AttributeDef const*> const& AttributeDef::GetAllAttributeDefs()
{
	return s_attributeDefs;
}



//----------------------------------------------------------------------------------------------------------------------
bool AttributeDef::GetIsPersistent() const
{
	return (m_tags & static_cast<uint8_t>(AttributeTag::Persistent)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool AttributeDef::GetIsState() const
{
	return (m_tags & static_cast<uint8_t>(AttributeTag::State)) != 0;
}



//----------------------------------------------------------------------------------------------------------------------
void AttributeDef::SetIsPersistent(bool isPersistent)
{
	if (isPersistent)
	{
		m_tags |= static_cast<uint8_t>(AttributeTag::Persistent);
	}
	else
	{
		m_tags &= ~static_cast<uint8_t>(AttributeTag::Persistent);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AttributeDef::SetIsState(bool isState)
{
	if (isState)
	{
		m_tags |= static_cast<uint8_t>(AttributeTag::State);
	}
	else
	{
		m_tags &= ~static_cast<uint8_t>(AttributeTag::State);
	}
}



//----------------------------------------------------------------------------------------------------------------------
AttributeDef::AttributeDef(void const* xmlElement)
{
    XmlElement const& elem = *reinterpret_cast<XmlElement const*>(xmlElement);
    m_name = XmlUtils::ParseXmlAttribute(elem, "name", m_name);

	bool persistent = XmlUtils::ParseXmlAttribute(elem, "persistent", false);
	bool state = XmlUtils::ParseXmlAttribute(elem, "state", false);

	SetIsPersistent(persistent);
	SetIsState(state);

	m_displayInfo.m_displayName = XmlUtils::ParseXmlAttribute(elem, "displayName", m_displayInfo.m_displayName);
	Name displayTypeName = XmlUtils::ParseXmlAttribute(elem, "displayType", Name::Invalid);

	if (displayTypeName == Name("Integer"))
	{
		m_displayInfo.m_displayType = AttributeDisplayType::Integer;
	}
	else if (displayTypeName == Name("IntegerSigned"))
	{
		m_displayInfo.m_displayType = AttributeDisplayType::IntegerSigned;
	}
	else if (displayTypeName == Name("Decimal"))
	{
		m_displayInfo.m_displayType = AttributeDisplayType::Decimal;
	}
	else if (displayTypeName == Name("DecimalSigned"))
	{
		m_displayInfo.m_displayType = AttributeDisplayType::DecimalSigned;
	}
	else if (displayTypeName == Name("Percent"))
	{
		m_displayInfo.m_displayType = AttributeDisplayType::Percent;
	}
	else if (displayTypeName == Name("PercentSigned"))
	{
		m_displayInfo.m_displayType = AttributeDisplayType::PercentSigned;
	}
	else if (displayTypeName == Name("Seconds"))
	{
		m_displayInfo.m_displayType = AttributeDisplayType::Seconds;
	}
}