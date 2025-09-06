// Bradley Christensen - 2022-2025
#include "XmlUtils.h"
#include "StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"



//----------------------------------------------------------------------------------------------------------------------
std::string XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const*	defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->Value();
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
std::string XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return std::string(attrib->Value());
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
Name XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, Name defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return Name(attrib->Value());
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
float XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->FloatValue();
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
bool XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->BoolValue();
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
int XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->IntValue();
    }
    return defaultValue; 
}



//----------------------------------------------------------------------------------------------------------------------
uint8_t XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, uint8_t defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        int intVal = attrib->IntValue();
        if (intVal < 0)
        {
			DevConsoleUtils::LogError("uint8_t ParseXmlAttribute Error: value is negative, but type is unsigned.");
        }
        return static_cast<uint8_t>(intVal);
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        std::string value = attrib->Value();
        auto strings = StringUtils::SplitStringOnDelimiter(value, ',');
        if (strings.size() != 2)
        {
			DevConsoleUtils::LogError("Vec2 ParseXmlAttribute Error: %s - value: %s - (e.g. -40,30.5)", attributeName, attrib->Value());
            return defaultValue;
        }
        Vec2 result;
        tinyxml2::XMLUtil::ToFloat(strings[0].c_str(), &result.x);
        tinyxml2::XMLUtil::ToFloat(strings[1].c_str(), &result.y);
        return result;
    }
    return defaultValue; 
}



//----------------------------------------------------------------------------------------------------------------------
IntVec2 XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        std::string value = attrib->Value();
        auto strings = StringUtils::SplitStringOnDelimiter(value, ',');
        if (strings.size() != 2)
        {
			DevConsoleUtils::LogError("IntVec2 ParseXmlAttribute Error: %s - value: %s - (e.g. -40,30)", attributeName, attrib->Value());
            return defaultValue;
        }
        IntVec2 result;
        tinyxml2::XMLUtil::ToInt(strings[0].c_str(), &result.x);
        tinyxml2::XMLUtil::ToInt(strings[1].c_str(), &result.y);
        return result;
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 XmlUtils::ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        std::string value = attrib->Value();
        Strings strings = StringUtils::SplitStringOnDelimiter(value, ',');
        if (strings.size() < 3 || strings.size() > 4)
        {
			DevConsoleUtils::LogError("Rgba8 ParseXmlAttribute Error: %s - value: %s - (e.g. 40,30,255 or 255,255,255,255)", attributeName, attrib->Value());
            return defaultValue;
        }

        Rgba8 result;
        uint32_t r, g, b, a;

        // Red
        tinyxml2::XMLUtil::ToUnsigned(strings[0].c_str(), &r);
        result.r = (uint8_t) r;

        // Green
        tinyxml2::XMLUtil::ToUnsigned(strings[1].c_str(), &g);
        result.g = (uint8_t) g;

        // Blue
        tinyxml2::XMLUtil::ToUnsigned(strings[2].c_str(), &b);
        result.b = (uint8_t) b;

        // Alpha
        if (strings.size() > 3)
        {
            tinyxml2::XMLUtil::ToUnsigned(strings[3].c_str(), &a);
            result.a = (uint8_t) a;
        }

        return result;
    }
    return defaultValue;
}
