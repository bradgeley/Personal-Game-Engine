// Bradley Christensen - 2022-2023
#include "XmlUtils.h"
#include "StringUtils.h"
#include "Engine/Debug/DevConsole.h"



//----------------------------------------------------------------------------------------------------------------------
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const*	defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->Value();
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->Value();
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
float ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->FloatValue();
    }
    return defaultValue;
}



//----------------------------------------------------------------------------------------------------------------------
int ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        return attrib->IntValue();
    }
    return defaultValue; 
}



//----------------------------------------------------------------------------------------------------------------------
Vec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue)
{
    if (XmlAttribute const* attrib = element.FindAttribute(attributeName))
    {
        std::string value = attrib->Value();
        auto strings = SplitStringOnDelimeter(value, ',');
        if (strings.size() != 2)
        {
            g_devConsole->LogErrorF("Vec2 ParseXmlAttribute Error: %s - value: %s - (e.g. -40,30.5)", attributeName, attrib->Value());
        }
        Vec2 result;
        tinyxml2::XMLUtil::ToFloat(strings[0].c_str(), &result.x);
        tinyxml2::XMLUtil::ToFloat(strings[1].c_str(), &result.y);
        return result;
        
    }
    return defaultValue; 
}
