// Bradley Christensen - 2022-2026
#include "RunModifierDef.h"
#include "Ability.h"
#include "CAbility.h"
#include "CTags.h"
#include "SCRunData.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/ErrorUtils.h"



//----------------------------------------------------------------------------------------------------------------------
RunModifierDef::RunModifierDef(XmlElement const& modElement)
{
	m_name = XmlUtils::ParseXmlAttribute(modElement, "name", m_name);
	m_displayName = XmlUtils::ParseXmlAttribute(modElement, "displayName", m_displayName);
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
	else if (modTypeName == "TowerPayloadRunModifier")
	{
		return new TowerPayloadRunModifierDef(modElement);
	}

	//ERROR_AND_DIE("Unknown RunModifierDef type: " + std::string(element.Name()));
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void RunModifierDef::AppendDebugString(std::string& outStr) const
{
	outStr += StringUtils::StringF("%s", m_name.ToCStr());
	if (m_maxLevel > 1)
	{
		outStr += StringUtils::StringF("\nMax Level: %d", m_maxLevel);
	}
	outStr += "\n";
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier::RunModifier(RunModifierDef const& def) : m_def(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void RunModifier::ApplyToAbility(Ability&, CTags const&) const
{

}



//----------------------------------------------------------------------------------------------------------------------
void RunModifier::AppendDebugString(std::string& outStr) const
{
	outStr += StringUtils::StringF("%s", m_def.m_name.ToCStr());
	if (m_def.m_maxLevel > 1)
	{
		outStr += StringUtils::StringF("\nLevel: %d/%d", m_level, m_def.m_maxLevel);
	}
	outStr += "\n";
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
void TowerUnlockRunModifierDef::AppendDebugString(std::string& outStr) const
{
	RunModifierDef::AppendDebugString(outStr);

	outStr += StringUtils::StringF("Gold Cost: %.2f\n", m_baseCost);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerUnlockRunModifierDef::GetDescription(std::string& outStr) const
{
	outStr += StringUtils::StringF("Gold Cost: %.2f\n", m_baseCost);
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
void TowerUnlockRunModifier::AppendDebugString(std::string& outStr) const
{
	RunModifier::AppendDebugString(outStr);

	outStr += StringUtils::StringF("Tower Name: %s\nGold Cost: %.2f\n", GetDef().m_towerName.ToCStr(), GetDef().m_baseCost);
}



//----------------------------------------------------------------------------------------------------------------------
TowerUnlockRunModifierDef const& TowerUnlockRunModifier::GetDef() const
{
	return static_cast<TowerUnlockRunModifierDef const&>(m_def);
}



//----------------------------------------------------------------------------------------------------------------------
TowerPayloadRunModifierDef::TowerPayloadRunModifierDef(XmlElement const& modElement) : RunModifierDef(modElement)
{
	m_multiplierIncreaseBase = XmlUtils::ParseXmlAttribute(modElement, "base", m_multiplierIncreaseBase);
	m_multiplierIncreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "perLevel", m_multiplierIncreasePerLevel);

	std::string payloadTypesString = XmlUtils::ParseXmlAttribute(modElement, "type", "");
	Strings payloadTypeStrings = StringUtils::SplitStringOnDelimiter(payloadTypesString, ',');

	for (std::string const& payloadTypeString : payloadTypeStrings)
	{
		if (Name(payloadTypeString) == "Damage")
		{
			m_payloadDamageTypeFlags |= static_cast<uint8_t>(PayloadType::Damage);
		}
		else if (Name(payloadTypeString) == "Slow")
		{
			m_payloadDamageTypeFlags |= static_cast<uint8_t>(PayloadType::Slow);
		}
		else if (Name(payloadTypeString) == "Burn")
		{
			m_payloadDamageTypeFlags |= static_cast<uint8_t>(PayloadType::Burn);
		}
		else if (Name(payloadTypeString) == "Poison")
		{
			m_payloadDamageTypeFlags |= static_cast<uint8_t>(PayloadType::Poison);
		}
		else
		{
			ASSERT_OR_DIE(false, StringUtils::StringF("Unknown payload damage type: %s", payloadTypeString.c_str()));
		}
	}

	std::string tagRequirementsString = XmlUtils::ParseXmlAttribute(modElement, "tagRequirement", "");
	Strings tagRequirementStrings = StringUtils::SplitStringOnDelimiter(tagRequirementsString, ',');

	ASSERT_OR_DIE(tagRequirementStrings.size() <= s_maxRequirements, StringUtils::StringF("RunModifierDef \"%s\" has too many tag requirements. Max is %d", m_name.ToString().c_str(), s_maxRequirements));

	for (size_t i = 0; i < tagRequirementStrings.size() && i < s_maxRequirements; ++i)
	{
		m_tagRequirements[i] = Name(tagRequirementStrings[i]);
	}
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier* TowerPayloadRunModifierDef::MakeModifierInstance() const
{
	return new TowerPayloadRunModifier(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerPayloadRunModifierDef::AppendDebugString(std::string& outStr) const
{
	RunModifierDef::AppendDebugString(outStr);

	outStr += StringUtils::StringF("Multiplier Increase Base: %.2f\nMultiplier Increase Per Level: %.2f\n", m_multiplierIncreaseBase, m_multiplierIncreasePerLevel);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerPayloadRunModifierDef::GetDescription(std::string& outStr) const
{
	outStr += StringUtils::StringF("Base: +%.2fx\nPer Level: +%.2fx\n", m_multiplierIncreaseBase, m_multiplierIncreasePerLevel);
}



//----------------------------------------------------------------------------------------------------------------------
TowerPayloadRunModifier::TowerPayloadRunModifier(TowerPayloadRunModifierDef const& def) : RunModifier(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void TowerPayloadRunModifier::Apply(SystemContext const& context) const
{
	auto& abilityStorage = context.GetMapStorage<CAbility>();
	auto const& tagStorage = context.GetArrayStorageConst<CTags>();

	for (auto it = context.Iterate<CAbility, CTags>(); it.IsValid(); it.Next())
	{
		CTags const& tagComponent = tagStorage[it];

		bool matchesTagRequirement = true;
		for (int tagIndex = 0; tagIndex < RunModifierDef::s_maxRequirements; ++tagIndex)
		{
			Name tagRequirement = GetDef().m_tagRequirements[tagIndex];
			if (tagRequirement != Name::Invalid && !tagComponent.HasTag(tagRequirement))
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

		for (Ability*& ability : abilityComponent.m_abilities)
		{
			ability->m_needsRebuild = true;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void TowerPayloadRunModifier::ApplyToAbility(Ability& ability, CTags const& tags) const
{
	ability.ApplyModifier(*this, tags);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerPayloadRunModifier::AppendDebugString(std::string& outStr) const
{
	RunModifier::AppendDebugString(outStr);

	outStr += StringUtils::StringF("Payload Multiplier Increase Base: %.2f\nPayload Multiplier Increase Per Level: %.2f\n", GetDef().m_multiplierIncreaseBase, GetDef().m_multiplierIncreasePerLevel);
}



//----------------------------------------------------------------------------------------------------------------------
TowerPayloadRunModifierDef const& TowerPayloadRunModifier::GetDef() const
{
	return static_cast<TowerPayloadRunModifierDef const&>(m_def);
}