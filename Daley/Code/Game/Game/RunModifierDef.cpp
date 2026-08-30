// Bradley Christensen - 2022-2026
#include "RunModifierDef.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
RunModifierDef::RunModifierDef(XmlElement const& element)
{
	m_name = XmlUtils::ParseXmlAttribute(element, "name", m_name);
	m_levelRequirement = XmlUtils::ParseXmlAttribute(element, "levelRequirement", m_levelRequirement);
	m_weight = XmlUtils::ParseXmlAttribute(element, "weight", m_weight);
	m_maxLevel = XmlUtils::ParseXmlAttribute(element, "maxLevel", m_maxLevel);

	std::string requirementsString = XmlUtils::ParseXmlAttribute(element, "requirements", "");
	Strings requirementStrings = StringUtils::SplitStringOnDelimiter(requirementsString, ',');

	for (size_t i = 0; i < requirementStrings.size() && i < s_maxRequirements; ++i)
	{
		m_requirements[i] = Name(requirementStrings[i]);
	}
}



//----------------------------------------------------------------------------------------------------------------------
RunModifierDef const* RunModifierDef::MakeFromXml(XmlElement const& element)
{
	Name elementName = Name(element.Name());

	if (elementName == "TowerUnlockRunModifier")
	{
		return new TowerUnlockRunModifierDef(element);
	}

	//ERROR_AND_DIE("Unknown RunModifierDef type: " + std::string(element.Name()));
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier::RunModifier(RunModifierDef const& def) : m_def(def)
{

}




//----------------------------------------------------------------------------------------------------------------------
TowerUnlockRunModifierDef::TowerUnlockRunModifierDef(XmlElement const& element) : RunModifierDef(element)
{
	m_towerName = XmlUtils::ParseXmlAttribute(element, "name", Name::Invalid);
	m_defaultKey = XmlUtils::ParseXmlAttribute(element, "defaultKey", m_defaultKey);
	m_baseCost = XmlUtils::ParseXmlAttribute(element, "baseCost", m_baseCost);
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier* TowerUnlockRunModifierDef::MakeModifierInstance() const
{
	return new TowerUnlockRunModifier(*this);
}



//----------------------------------------------------------------------------------------------------------------------
TowerUnlockRunModifier::TowerUnlockRunModifier(TowerUnlockRunModifierDef const& def) : RunModifier(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void TowerUnlockRunModifier::Apply(SystemContext const& context) const
{
	SCRunData& runData = context.GetSingleton<SCRunData>();
	TowerUnlockRunModifierDef const& def = GetDef();

	for (PlaceableTower& placeableTower : runData.m_data->m_placeableTowers)
	{
		if (placeableTower.m_towerName == Name::Invalid)
		{
			// Reached an invalid slot before finding the tower, so we cannot already place this tower, add.
			placeableTower.m_towerName = def.m_towerName;
			placeableTower.m_hotkey = def.m_defaultKey;
			placeableTower.m_cost = def.m_baseCost;
			return;
		}
		else if (placeableTower.m_towerName == def.m_towerName)
		{
			// Already can place this tower
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
TowerUnlockRunModifierDef const& TowerUnlockRunModifier::GetDef() const
{
	return static_cast<TowerUnlockRunModifierDef const&>(m_def);
}
