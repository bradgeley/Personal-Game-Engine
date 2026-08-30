// Bradley Christensen - 2022-2026
#include "RunModifierDef.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
RunModifierDef::RunModifierDef(XmlElement const& modElement)
{
	m_name = XmlUtils::ParseXmlAttribute(modElement, "name", m_name);
	m_levelRequirement = XmlUtils::ParseXmlAttribute(modElement, "levelRequirement", m_levelRequirement);
	m_weight = XmlUtils::ParseXmlAttribute(modElement, "weight", m_weight);
	m_maxLevel = XmlUtils::ParseXmlAttribute(modElement, "maxLevel", m_maxLevel);

	ASSERT_OR_DIE(modElement.Parent() != nullptr, "RunModifierDef XML element has no parent element.");
	ASSERT_OR_DIE(modElement.Parent()->ToElement() != nullptr, "RunModifierDef XML element's parent is not a valid XML element.");
	ASSERT_OR_DIE(Name(modElement.Parent()->ToElement()->Name()) == Name("RunModifierPool"), "RunModifierDef XML element is not a RunModifierPool.");
	XmlElement const& poolElement = *modElement.Parent()->ToElement();

	std::string poolRequirementsString = XmlUtils::ParseXmlAttribute(poolElement, "requirements", "");
	Strings poolRequirementStrings = StringUtils::SplitStringOnDelimiter(poolRequirementsString, ',');

	std::string modRequirementsString = XmlUtils::ParseXmlAttribute(modElement, "requirements", "");
	Strings modRequirementStrings = StringUtils::SplitStringOnDelimiter(modRequirementsString, ',');

	Strings& combinedRequirements = poolRequirementStrings;
	combinedRequirements.insert(combinedRequirements.end(), modRequirementStrings.begin(), modRequirementStrings.end());

	ASSERT_OR_DIE(combinedRequirements.size() <= s_maxRequirements, StringUtils::StringF("RunModifierDef \"%s\" has too many requirements. Max is %d", m_name.ToString().c_str(), s_maxRequirements));

	for (size_t i = 0; i < combinedRequirements.size() && i < s_maxRequirements; ++i)
	{
		m_requirements[i] = Name(combinedRequirements[i]);
	}
}



//----------------------------------------------------------------------------------------------------------------------
RunModifierDef const* RunModifierDef::MakeFromXml(XmlElement const& modElement)
{
	Name modTypeName = Name(modElement.Name());

	if (modTypeName == "TowerUnlockRunModifier")
	{
		return new TowerUnlockRunModifierDef(modElement);
	}

	//ERROR_AND_DIE("Unknown RunModifierDef type: " + std::string(element.Name()));
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier::RunModifier(RunModifierDef const& def) : m_def(def)
{

}




//----------------------------------------------------------------------------------------------------------------------
TowerUnlockRunModifierDef::TowerUnlockRunModifierDef(XmlElement const& modElement) : RunModifierDef(modElement)
{
	m_towerName = XmlUtils::ParseXmlAttribute(modElement, "name", Name::Invalid);
	m_defaultKey = XmlUtils::ParseXmlAttribute(modElement, "defaultKey", m_defaultKey);
	m_baseCost = XmlUtils::ParseXmlAttribute(modElement, "baseCost", m_baseCost);
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
