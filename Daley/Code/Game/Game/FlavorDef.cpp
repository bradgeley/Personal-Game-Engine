// Bradley Christensen - 2022-2026
#include "FlavorDef.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
static const char* s_flavorDefsFilePath = "Data/Definitions/FlavorDefs.xml";
std::vector<FlavorDef> FlavorDef::s_flavorDefs;



//----------------------------------------------------------------------------------------------------------------------
FlavorDef::FlavorDef(XmlElement const* flavorDefXmlElement)
{
	m_name = XmlUtils::ParseXmlAttribute(*flavorDefXmlElement, "name", m_name);

	// Abilities
	std::string abilitiesString = XmlUtils::ParseXmlAttribute(*flavorDefXmlElement, "abilities", "");
	Strings abilityNames = StringUtils::SplitStringOnDelimiter(abilitiesString, ',');
	ASSERT_OR_DIE(abilityNames.size() <= s_maxAbilities, StringUtils::StringF("FlavorDef \"%s\" has too many abilities. Max is %d", m_name.ToCStr(), s_maxAbilities).c_str());
	
	m_abilities.fill(Name::Invalid);
	for (size_t i = 0; i < abilityNames.size() && i < s_maxAbilities; ++i)
	{
		m_abilities[i] = Name(abilityNames[i]);
	}

	// Attributes
	if (XmlElement const* attributesElement = flavorDefXmlElement->FirstChildElement("Attributes"))
	{
		m_attributes = AbilityAttributes(attributesElement);
	}

	// Ability Flags
	if (XmlElement const* flagsElement = flavorDefXmlElement->FirstChildElement("AbilityFlags"))
	{
		m_abilityFlags = AbilityFlags(flagsElement);
	}
	
	// Display Data
	if (XmlElement const* displayDataElement = flavorDefXmlElement->FirstChildElement("DisplayData"))
	{
		m_displayName = XmlUtils::ParseXmlAttribute(*displayDataElement, "displayName", m_displayName);
		m_spriteSheetName = Name(XmlUtils::ParseXmlAttribute(*displayDataElement, "spriteSheet", m_spriteSheetName.ToString()));
		m_animName = Name(XmlUtils::ParseXmlAttribute(*displayDataElement, "anim", m_animName.ToString()));
		m_tint = XmlUtils::ParseXmlAttribute(*displayDataElement, "tint", m_tint);
		m_desc = XmlUtils::ParseXmlAttribute(*displayDataElement, "desc", m_desc);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorDef::LoadFromXML()
{
	if (s_flavorDefs.empty())
	{
		XmlDocument flavorDefinitionsDoc;
		flavorDefinitionsDoc.LoadFile(s_flavorDefsFilePath);
		XmlElement const* flavorDefElement = flavorDefinitionsDoc.RootElement()->FirstChildElement();

		while (flavorDefElement)
		{
			s_flavorDefs.emplace_back(flavorDefElement);
			flavorDefElement = flavorDefElement->NextSiblingElement();
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorDef::Shutdown()
{
	s_flavorDefs.clear();
}



//----------------------------------------------------------------------------------------------------------------------
FlavorDef const* FlavorDef::GetFlavorDef(Name name)
{
	for (size_t i = 0; i < s_flavorDefs.size(); i++)
	{
		FlavorDef const* def = &s_flavorDefs[i];
		if (def->m_name == name)
		{
			return def;
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<FlavorDef> const& FlavorDef::GetAllFlavorDefs()
{
	return s_flavorDefs;
}
