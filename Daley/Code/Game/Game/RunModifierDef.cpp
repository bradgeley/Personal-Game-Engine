// Bradley Christensen - 2022-2026
#include "RunModifierDef.h"
#include "AttributeDef.h"
#include "AttributeSet.h"
#include "CAbility.h"
#include "CTags.h"
#include "FlavorDef.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
RunModifierDisplayData::RunModifierDisplayData(XmlElement const& element)
{
	m_displayName = XmlUtils::ParseXmlAttribute(element, "displayName", m_displayName);
	m_spriteSheet = XmlUtils::ParseXmlAttribute(element, "spriteSheet", m_spriteSheet);
	m_anim = XmlUtils::ParseXmlAttribute(element, "anim", m_anim);
	m_tint = XmlUtils::ParseXmlAttribute(element, "tint", m_tint);
}



//----------------------------------------------------------------------------------------------------------------------
RunModifierDef::RunModifierDef(XmlElement const& modElement)
{
	m_name = XmlUtils::ParseXmlAttribute(modElement, "name", m_name);
	m_levelRequirement = XmlUtils::ParseXmlAttribute(modElement, "levelRequirement", m_levelRequirement);
	m_weight = XmlUtils::ParseXmlAttribute(modElement, "weight", m_weight);
	m_maxLevel = XmlUtils::ParseXmlAttribute(modElement, "maxLevel", m_maxLevel);

	XmlElement const* displayDataElement = modElement.FirstChildElement("DisplayData");
	if (displayDataElement != nullptr)
	{
		m_displayData = RunModifierDisplayData(*displayDataElement);

		displayDataElement = displayDataElement->NextSiblingElement("DisplayData");
		if (displayDataElement != nullptr)
		{
			m_secondaryDisplayData = RunModifierDisplayData(*displayDataElement);
		}
	}

	ASSERT_OR_DIE(modElement.Parent() != nullptr, "RunModifierDef XML element has no parent element.");
	ASSERT_OR_DIE(modElement.Parent()->ToElement() != nullptr, "RunModifierDef XML element's parent is not a valid XML element.");
	ASSERT_OR_DIE(Name(modElement.Parent()->ToElement()->Name()) == Name("RunModifierPool"), "RunModifierDef XML element is not a RunModifierPool.");
	XmlElement const& poolElement = *modElement.Parent()->ToElement();

	std::string poolRequirementsString = XmlUtils::ParseXmlAttribute(poolElement, "requirements", "");
	Strings poolRequirementStrings = StringUtils::SplitStringOnDelimiter(poolRequirementsString, ',');

	std::string modRequirementsString = XmlUtils::ParseXmlAttribute(modElement, "requirements", "");
	Strings modRequirementStrings = StringUtils::SplitStringOnDelimiter(modRequirementsString, ',');

	Strings combinedRequirements = poolRequirementStrings;
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

	if (modTypeName == "FlavorUnlockRunModifier")
	{
		return new FlavorUnlockRunModifierDef(modElement);
	}
	else if (modTypeName == "FlavorAbilityRunModifier")
	{
		return new FlavorAbilityRunModifierDef(modElement);
	}
	else if (modTypeName == "MetaRunModifier")
	{
		return new MetaRunModifierDef(modElement);
	}

	ERROR_AND_DIE(StringUtils::StringF("Unknown RunModifierDef type: %s", modElement.Name()));
}



//----------------------------------------------------------------------------------------------------------------------
bool IsActiveModRequirementMet(Name requirement, std::vector<RunModifier*> const& activeModifiers)
{
	if (requirement == Name::Invalid)
	{
		return true;
	}
	for (RunModifier const* activeModifier : activeModifiers)
	{
		if (activeModifier->m_def.m_name == requirement)
		{
			return true;
		}
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool RunModifierDef::AreRequirementsMet(RunData const& runData) const
{
	ExperienceLevelData levelData = runData.GetLevelData(runData.m_experience);
	if (m_levelRequirement > levelData.m_level)
	{
		return false;
	}

	for (Name requirement : m_requirements)
	{
		if (!IsActiveModRequirementMet(requirement, runData.m_activeRunModifiers))
		{
			return false;
		}
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier::RunModifier(RunModifierDef const& def) : m_def(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void RunModifier::ApplyToAbility(CAbility&, CTags const&) const
{
	// Empty Base
}



//----------------------------------------------------------------------------------------------------------------------
void RunModifier::ApplyToRunData(RunData&) const
{
	// Empty Base
}




//----------------------------------------------------------------------------------------------------------------------
FlavorUnlockRunModifierDef::FlavorUnlockRunModifierDef(XmlElement const& modElement) : RunModifierDef(modElement)
{
	m_flavorName = XmlUtils::ParseXmlAttribute(modElement, "name", Name::Invalid);
	m_forcedKey = XmlUtils::ParseXmlAttribute(modElement, "forcedKey", m_forcedKey);
	m_baseCost = XmlUtils::ParseXmlAttribute(modElement, "baseCost", m_baseCost);
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier* FlavorUnlockRunModifierDef::MakeModifierInstance() const
{
	return new FlavorUnlockRunModifier(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorUnlockRunModifierDef::GetDescription(std::string& outStr) const
{
	outStr += StringUtils::StringF("Gold Cost: %.2f\n", m_baseCost);

	FlavorDef const* flavorDef = FlavorDef::GetFlavorDef(m_flavorName);
	if (flavorDef)
	{
		outStr += StringUtils::StringF("%s\n", flavorDef->m_desc.c_str());
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool FlavorUnlockRunModifierDef::AreRequirementsMet(RunData const& runData) const
{
	if (!RunModifierDef::AreRequirementsMet(runData))
	{
		return false;
	}

	int numStartingTowers = 1; // Wall1x1
	int numFlavors = 0;
	for (PlaceableTower const& placeableTower : runData.m_placeableTowers)
	{
		if (placeableTower.m_towerName != Name::Invalid)
		{
			numFlavors++;
		}
	}

	int maxFlavors = static_cast<int>(runData.GetAttribute("FlavorLimit", 0.f));
	int maxNumPlaceables = (maxFlavors + numStartingTowers);
	if (numFlavors >= maxNumPlaceables)
	{
		return false;
	}

	return true;
}



//----------------------------------------------------------------------------------------------------------------------
FlavorUnlockRunModifier::FlavorUnlockRunModifier(FlavorUnlockRunModifierDef const& def) : RunModifier(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void FlavorUnlockRunModifier::Apply(SystemContext const& context) const
{
	RunData& runData = *context.GetSingleton<SCRunData>().m_data;
	ApplyToRunData(runData);
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorUnlockRunModifier::ApplyToRunData(RunData& runData) const
{
	FlavorUnlockRunModifierDef const& def = GetDef();

	for (int placeableIndex = 0; placeableIndex < runData.m_placeableTowers.size(); ++placeableIndex)
	{
		PlaceableTower& placeableTower = runData.m_placeableTowers[placeableIndex];

		if (placeableTower.m_towerName == Name::Invalid)
		{
			// Reached an invalid slot before finding the tower, so we cannot already place this tower, add.
			placeableTower.m_towerName = "Tower2x2";
			placeableTower.m_flavorName = def.m_flavorName;
			placeableTower.m_hotkey = runData.m_towerPlacementKeyBindings[placeableIndex];
			placeableTower.m_baseCost = def.m_baseCost;
			placeableTower.m_cost = def.m_baseCost;
			return;
		}
		else if (placeableTower.m_flavorName == def.m_flavorName && placeableTower.m_towerName == "Tower2x2")
		{
			// Already can place this tower
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorUnlockRunModifier::GetDescription(std::string& outStr) const
{
	outStr += StringUtils::StringF("Tower Name: %s\nGold Cost: %.2f\n", GetDef().m_flavorName.ToCStr(), GetDef().m_baseCost);
}



//----------------------------------------------------------------------------------------------------------------------
FlavorUnlockRunModifierDef const& FlavorUnlockRunModifier::GetDef() const
{
	return static_cast<FlavorUnlockRunModifierDef const&>(m_def);
}



//----------------------------------------------------------------------------------------------------------------------
FlavorAbilityRunModifierDef::FlavorAbilityRunModifierDef(XmlElement const& modElement) : RunModifierDef(modElement)
{
	if (XmlElement const* baseAttributesElem = modElement.FirstChildElement("Attributes"))
	{
		m_modifiers = AttributeSet(baseAttributesElem);
	}

	if (XmlElement const* perLevelAttributesElem = modElement.FirstChildElement("AttributesPerLevel"))
	{
		m_modifiersPerLevel = AttributeSet(perLevelAttributesElem);
	}

	std::string payloadTypesString = XmlUtils::ParseXmlAttribute(modElement, "type", "");
	Strings payloadTypeStrings = StringUtils::SplitStringOnDelimiter(payloadTypesString, ',');

	std::string tagRequirementsString = XmlUtils::ParseXmlAttribute(modElement, "tagRequirement", "");
	Strings tagRequirementStrings = StringUtils::SplitStringOnDelimiter(tagRequirementsString, ',');

	ASSERT_OR_DIE(tagRequirementStrings.size() <= s_maxRequirements, StringUtils::StringF("RunModifierDef \"%s\" has too many tag requirements. Max is %d", m_name.ToString().c_str(), s_maxRequirements));

	for (size_t i = 0; i < tagRequirementStrings.size() && i < s_maxRequirements; ++i)
	{
		m_tagRequirements[i] = Name(tagRequirementStrings[i]);
	}
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier* FlavorAbilityRunModifierDef::MakeModifierInstance() const
{
	return new FlavorAbilityRunModifier(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorAbilityRunModifierDef::GetDescription(std::string& outStr) const
{
	for (int attributeIndex = 0; attributeIndex < m_modifiers.m_count; ++attributeIndex)
	{
		float firstValue = m_modifiers.m_values[attributeIndex];
		if (firstValue == 0.f)
		{
			continue;
		}

		std::string formattedValue = AttributeSet::FormatAttribute(m_modifiers.m_keys[attributeIndex], firstValue);

		outStr += StringUtils::StringF("Value: %s\n", formattedValue.c_str());
	}

	if (m_maxLevel > 1)
	{
		outStr += StringUtils::StringF("Max Level: %d", m_maxLevel);
	}
}



//----------------------------------------------------------------------------------------------------------------------
FlavorAbilityRunModifier::FlavorAbilityRunModifier(FlavorAbilityRunModifierDef const& def) : RunModifier(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
AttributeSet FlavorAbilityRunModifier::GetValue() const
{
	FlavorAbilityRunModifierDef const& def = GetDef();
	return def.m_modifiers + (def.m_modifiersPerLevel * static_cast<float>(m_level - 1));
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorAbilityRunModifier::Apply(SystemContext const& context) const
{
	auto& abilityStorage = context.GetMapStorage<CAbility>();
	auto const& tagStorage = context.GetArrayStorageConst<CTags>();

	for (auto it = context.Iterate<CAbility, CTags>(); it.IsValid(); it.Next())
	{
		CTags const& tagComponent = tagStorage[it];

		bool matchesTagRequirement = true;
		for (int tagIndex = 0; tagIndex < RunModifierDef::s_maxRequirements; ++tagIndex)
		{
			Name const& tagRequirement = GetDef().m_tagRequirements[tagIndex];
			if (tagRequirement.IsValid() && !tagComponent.HasTag(tagRequirement))
			{
				matchesTagRequirement = false;
				break;
			}
		}

		if (!matchesTagRequirement)
		{
			continue;
		}

		CAbility& abilityComponent = abilityStorage[it];
		abilityComponent.m_needsAttributeRebuild = true;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorAbilityRunModifier::ApplyToAbility(CAbility& ability, CTags const& tags) const
{
	auto& def = GetDef();

	for (Name const& tag : def.m_tagRequirements)
	{
		if (tag.IsValid() && !tags.HasTag(tag))
		{
			return;
		}
	}

	AttributeSet valuesToAdd = def.m_modifiers + (def.m_modifiersPerLevel * static_cast<float>(m_level - 1));

	ability.m_attributes += valuesToAdd;
}



//----------------------------------------------------------------------------------------------------------------------
void FlavorAbilityRunModifier::GetDescription(std::string& outStr) const
{
	AttributeSet const& perLevel = GetDef().m_modifiersPerLevel;
	AttributeSet const currentValues = GetValue();
	AttributeSet const nextValues = GetDef().m_modifiers + (perLevel * static_cast<float>(m_level));

	// Only the attributes that scale per level can change, so they are the only ones worth showing
	for (int i = 0; i < perLevel.m_count; ++i)
	{
		if (perLevel.m_values[i] == 0.f)
		{
			continue;
		}

		Name const& key = perLevel.m_keys[i];

		AttributeDef const* def = AttributeDef::GetAttributeDef(key);
		char const* displayName = def ? def->m_displayInfo.m_displayName.ToCStr() : key.ToCStr();

		std::string const currentFormatted = AttributeSet::FormatAttribute(key, currentValues.GetValue(key));
		std::string const nextFormatted = AttributeSet::FormatAttribute(key, nextValues.GetValue(key));

		outStr += StringUtils::StringF("%s: %s -> %s\n", displayName, currentFormatted.c_str(), nextFormatted.c_str());
	}
}



//----------------------------------------------------------------------------------------------------------------------
FlavorAbilityRunModifierDef const& FlavorAbilityRunModifier::GetDef() const
{
	return static_cast<FlavorAbilityRunModifierDef const&>(m_def);
}



//----------------------------------------------------------------------------------------------------------------------
MetaRunModifierDef::MetaRunModifierDef(XmlElement const& modElement) : RunModifierDef(modElement)
{
	if (XmlElement const* baseAttributesElem = modElement.FirstChildElement("Attributes"))
	{
		m_modifiers = AttributeSet(baseAttributesElem);
	}

	if (XmlElement const* perLevelAttributesElem = modElement.FirstChildElement("AttributesPerLevel"))
	{
		m_modifiersPerLevel = AttributeSet(perLevelAttributesElem);
	}
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier* MetaRunModifierDef::MakeModifierInstance() const
{
	return new MetaRunModifier(*this);
}


//----------------------------------------------------------------------------------------------------------------------
void MetaRunModifierDef::GetDescription(std::string& outStr) const
{
	for (int i = 0; i < m_modifiers.m_count; ++i)
	{
		Name const& key = m_modifiers.m_keys[i];
		float const baseValue = m_modifiers.m_values[i];
		float const perLevelValue = m_modifiersPerLevel.GetValue(key);

		if (baseValue == 0.f && perLevelValue == 0.f)
		{
			continue;
		}

		AttributeDef const* attributeDef = AttributeDef::GetAttributeDef(key);
		Name displayName = attributeDef ? attributeDef->m_displayInfo.m_displayName : key;

		std::string const baseFormatted = AttributeSet::FormatAttribute(key, baseValue);
		outStr += StringUtils::StringF("%s: %s\n", displayName.ToCStr(), baseFormatted.c_str());

		if (m_maxLevel > 1 && perLevelValue != 0.f)
		{
			std::string const perLevelFormatted = AttributeSet::FormatAttribute(key, perLevelValue);
			outStr += StringUtils::StringF("Per Level: %s\n", perLevelFormatted.c_str());
		}
	}

	if (m_maxLevel > 1)
	{
		outStr += StringUtils::StringF("Max Level: %d", m_maxLevel);
	}
}



//----------------------------------------------------------------------------------------------------------------------
MetaRunModifier::MetaRunModifier(MetaRunModifierDef const& def) : RunModifier(def)
{
	
}



//----------------------------------------------------------------------------------------------------------------------
AttributeSet MetaRunModifier::GetValue() const
{
	MetaRunModifierDef const& def = GetDef();
	return def.m_modifiers + (def.m_modifiersPerLevel * static_cast<float>(m_level - 1));
}



//----------------------------------------------------------------------------------------------------------------------
void MetaRunModifier::Apply(SystemContext const& context) const
{
	SCRunData& scRunData = context.GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;

	runData.m_needsModifierRecalculation = true;
}



//----------------------------------------------------------------------------------------------------------------------
void MetaRunModifier::ApplyToRunData(RunData& runData) const
{
	runData.m_attributes += GetValue();
}



//----------------------------------------------------------------------------------------------------------------------
void MetaRunModifier::GetDescription(std::string& outStr) const
{
	MetaRunModifierDef const& def = GetDef();
	AttributeSet const currentValues = GetValue();

	if (def.m_maxLevel > 1)
	{
		AttributeSet const& perLevel = def.m_modifiersPerLevel;
		AttributeSet const nextValues = def.m_modifiers + (perLevel * static_cast<float>(m_level));

		// Only the attributes that scale per level can change, so they are the only ones worth showing
		for (int i = 0; i < perLevel.m_count; ++i)
		{
			if (perLevel.m_values[i] == 0.f)
			{
				continue;
			}

			Name const& key = perLevel.m_keys[i];

			AttributeDef const* attributeDef = AttributeDef::GetAttributeDef(key);
			char const* displayName = attributeDef ? attributeDef->m_displayInfo.m_displayName.ToCStr() : key.ToCStr();

			std::string const currentFormatted = AttributeSet::FormatAttribute(key, currentValues.GetValue(key));
			std::string const nextFormatted = AttributeSet::FormatAttribute(key, nextValues.GetValue(key));

			outStr += StringUtils::StringF("%s: %s -> %s\n", displayName, currentFormatted.c_str(), nextFormatted.c_str());
		}

		outStr += StringUtils::StringF("Level: %d/%d\n", m_level, def.m_maxLevel);
	}
	else
	{
		for (int i = 0; i < currentValues.m_count; ++i)
		{
			if (currentValues.m_values[i] == 0.f)
			{
				continue;
			}

			Name const& key = currentValues.m_keys[i];

			AttributeDef const* attributeDef = AttributeDef::GetAttributeDef(key);
			char const* displayName = attributeDef ? attributeDef->m_displayInfo.m_displayName.ToCStr() : key.ToCStr();

			std::string const formatted = AttributeSet::FormatAttribute(key, currentValues.GetValue(key));
			outStr += StringUtils::StringF("%s: %s\n", displayName, formatted.c_str());
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
MetaRunModifierDef const& MetaRunModifier::GetDef() const
{
	return static_cast<MetaRunModifierDef const&>(m_def);
}