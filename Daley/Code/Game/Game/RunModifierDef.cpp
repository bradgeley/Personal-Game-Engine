// Bradley Christensen - 2022-2026
#include "RunModifierDef.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
RunModifierDef const* RunModifierDef::MakeFromXml(XmlElement const& element)
{
	Name elementName = Name(element.Name());

	if (elementName == "TowerRunModifier")
	{
		return new TowerRunModifierDef(element);
	}

	ERROR_AND_DIE("Unknown RunModifierDef type: " + std::string(element.Name()));
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TowerRunModifierDef::TowerRunModifierDef(XmlElement const& element)
{
	m_towerName = XmlUtils::ParseXmlAttribute(element, "name", Name::Invalid);
	m_defaultKey = XmlUtils::ParseXmlAttribute(element, "defaultKey", m_defaultKey);
}	