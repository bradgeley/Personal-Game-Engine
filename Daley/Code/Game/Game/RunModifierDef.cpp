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
	m_description = XmlUtils::ParseXmlAttribute(modElement, "desc", m_description);
	m_levelRequirement = XmlUtils::ParseXmlAttribute(modElement, "levelRequirement", m_levelRequirement);
	m_weight = XmlUtils::ParseXmlAttribute(modElement, "weight", m_weight);
	m_maxLevel = XmlUtils::ParseXmlAttribute(modElement, "maxLevel", m_maxLevel);

	XmlElement const* displayDataElement = modElement.FirstChildElement("DisplayData");
	if (displayDataElement != nullptr)
	{
		m_displayData = RunModifierDisplayData(*displayDataElement);
	}

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
	else if (modTypeName == "TowerAbilityRunModifier")
	{
		return new TowerAbilityRunModifierDef(modElement);
	}
	else if (modTypeName == "EconomyRunModifier")
	{
		return new EconomyRunModifierDef(modElement);
	}

	//ERROR_AND_DIE("Unknown RunModifierDef type: " + std::string(element.Name()));
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier::RunModifier(RunModifierDef const& def) : m_def(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void RunModifier::ApplyToAbility(Ability&, CTags const&) const
{
	// Empty Base
}



//----------------------------------------------------------------------------------------------------------------------
void RunModifier::ApplyToRunData(RunData&) const
{
	// Empty Base
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
void TowerUnlockRunModifierDef::GetDescription(std::string& outStr) const
{
	outStr += StringUtils::StringF("Gold Cost: %.2f\n", m_baseCost);
	if (m_description != Name::Invalid)
	{
		outStr += StringUtils::StringF("%s\n", m_description.ToCStr());
	}
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
void TowerUnlockRunModifier::GetDescription(std::string& outStr) const
{
	outStr += StringUtils::StringF("Tower Name: %s\nGold Cost: %.2f\n", GetDef().m_towerName.ToCStr(), GetDef().m_baseCost);
}



//----------------------------------------------------------------------------------------------------------------------
TowerUnlockRunModifierDef const& TowerUnlockRunModifier::GetDef() const
{
	return static_cast<TowerUnlockRunModifierDef const&>(m_def);
}



//----------------------------------------------------------------------------------------------------------------------
TowerAbilityRunModifierDef::TowerAbilityRunModifierDef(XmlElement const& modElement) : RunModifierDef(modElement)
{
	m_valueBase = XmlUtils::ParseXmlAttribute(modElement, "base", m_valueBase);
	m_valueIncreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "perLevel", m_valueIncreasePerLevel);

	std::string payloadTypesString = XmlUtils::ParseXmlAttribute(modElement, "type", "");
	Strings payloadTypeStrings = StringUtils::SplitStringOnDelimiter(payloadTypesString, ',');

	for (std::string const& payloadTypeString : payloadTypeStrings)
	{
		if (Name(payloadTypeString) == "Damage")
		{
			m_abilityAttribute = TowerAbilityAttribute::Damage;
		}
		else if (Name(payloadTypeString) == "Slow")
		{
			m_abilityAttribute = TowerAbilityAttribute::Slow;
		}
		else if (Name(payloadTypeString) == "Burn")
		{
			m_abilityAttribute = TowerAbilityAttribute::Burn;
		}
		else if (Name(payloadTypeString) == "Poison")
		{
			m_abilityAttribute = TowerAbilityAttribute::Poison;
		}
		else if (Name(payloadTypeString) == "Haste")
		{
			m_abilityAttribute = TowerAbilityAttribute::Haste;
		}
		else if (Name(payloadTypeString) == "AttackSpeed")
		{
			m_abilityAttribute = TowerAbilityAttribute::AttackSpeed;
		}
		else if (Name(payloadTypeString) == "CritChance")
		{
			m_abilityAttribute = TowerAbilityAttribute::CritChance;
		}
		else if (Name(payloadTypeString) == "CritDamage")
		{
			m_abilityAttribute = TowerAbilityAttribute::CritDamage;
		}
		else if (Name(payloadTypeString) == "AoE")
		{
			m_abilityAttribute = TowerAbilityAttribute::AoE;
		}
		else if (Name(payloadTypeString) == "MultiShot")
		{
			m_abilityAttribute = TowerAbilityAttribute::Multishot;
		}
		else if (Name(payloadTypeString) == "Chain")
		{
			m_abilityAttribute = TowerAbilityAttribute::NumChains;
		}
		else if (Name(payloadTypeString) == "Range")
		{
			m_abilityAttribute = TowerAbilityAttribute::Range;
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
RunModifier* TowerAbilityRunModifierDef::MakeModifierInstance() const
{
	return new TowerAbilityRunModifier(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerAbilityRunModifierDef::GetDescription(std::string& outStr) const
{
	float baseMultiplier = 1.f + m_valueBase;
	outStr += StringUtils::StringF("Base: %.2fx\nPer Level: +%.2fx\n", baseMultiplier, m_valueIncreasePerLevel);
	if (m_maxLevel > 1)
	{
		outStr += StringUtils::StringF("Max Level: %d", m_maxLevel);
	}
}



//----------------------------------------------------------------------------------------------------------------------
TowerAbilityRunModifier::TowerAbilityRunModifier(TowerAbilityRunModifierDef const& def) : RunModifier(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
float TowerAbilityRunModifier::GetValue() const
{
	TowerAbilityRunModifierDef const& def = GetDef();
	return def.m_valueBase + (def.m_valueIncreasePerLevel * static_cast<float>(m_level - 1));
}



//----------------------------------------------------------------------------------------------------------------------
void TowerAbilityRunModifier::Apply(SystemContext const& context) const
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
void TowerAbilityRunModifier::ApplyToAbility(Ability& ability, CTags const& tags) const
{
	for (auto& tag : GetDef().m_tagRequirements)
	{
		if (!tags.HasTag(tag))
		{
			return;
		}
	}

	ability.ApplyModifier(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerAbilityRunModifier::GetDescription(std::string& outStr) const
{
	if (m_def.m_maxLevel > 1)
	{
		float currentMultiplier = 1.f + GetValue();
		float nextMultiplier = currentMultiplier + GetDef().m_valueIncreasePerLevel;
		outStr += StringUtils::StringF("Current: %.2fx\nNext: %.2fx\nLevel: %d/%d\n", currentMultiplier, nextMultiplier, m_level, m_def.m_maxLevel);
	}
	else
	{
		float currentMultiplier = 1.f + GetDef().m_valueBase;
		outStr += StringUtils::StringF("Multiplier (additive): %.2fx\n", currentMultiplier);
	}
}



//----------------------------------------------------------------------------------------------------------------------
TowerAbilityRunModifierDef const& TowerAbilityRunModifier::GetDef() const
{
	return static_cast<TowerAbilityRunModifierDef const&>(m_def);
}



//----------------------------------------------------------------------------------------------------------------------
EconomyRunModifierDef::EconomyRunModifierDef(XmlElement const& modElement) : RunModifierDef(modElement)
{
	// Gold gain
	m_goldGainMultiplierIncreaseBase = XmlUtils::ParseXmlAttribute(modElement, "goldGainBase", m_goldGainMultiplierIncreaseBase);
	m_goldGainMultiplierIncreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "goldGainPerLevel", m_goldGainMultiplierIncreasePerLevel);

	// Interest
	m_interestRateIncreaseBase = XmlUtils::ParseXmlAttribute(modElement, "interestBase", m_interestRateIncreaseBase);
	m_interestRateIncreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "interestPerLevel", m_interestRateIncreasePerLevel);
	m_interestTimerSpeedIncreaseBase = XmlUtils::ParseXmlAttribute(modElement, "interestTimerSpeedBase", m_interestTimerSpeedIncreaseBase);
	m_interestTimerSpeedIncreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "interestTimerSpeedPerLevel", m_interestTimerSpeedIncreasePerLevel);

	// Selling
	m_baseSells = XmlUtils::ParseXmlAttribute(modElement, "baseSells", m_baseSells);
	m_sellsPerLevel = XmlUtils::ParseXmlAttribute(modElement, "sellsPerLevel", m_sellsPerLevel);
	m_baseSellRateIncrease = XmlUtils::ParseXmlAttribute(modElement, "baseSellRateIncrease", m_baseSellRateIncrease);
	m_sellRateIncreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "sellRateIncreasePerLevel", m_sellRateIncreasePerLevel);

	// Debt
	m_creditLimitIncreaseBase = XmlUtils::ParseXmlAttribute(modElement, "creditLimitBase", m_creditLimitIncreaseBase);
	m_creditLimitIncreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "creditLimitPerLevel", m_creditLimitIncreasePerLevel);
	m_debtInterestRateDecreaseBase = XmlUtils::ParseXmlAttribute(modElement, "debtInterestRateBase", m_debtInterestRateDecreaseBase);
	m_debtInterestRateDecreasePerLevel = XmlUtils::ParseXmlAttribute(modElement, "debtInterestRatePerLevel", m_debtInterestRateDecreasePerLevel);
}



//----------------------------------------------------------------------------------------------------------------------
RunModifier* EconomyRunModifierDef::MakeModifierInstance() const
{
	return new EconomyRunModifier(*this);
}



//----------------------------------------------------------------------------------------------------------------------
void EconomyRunModifierDef::GetDescription(std::string& outStr) const
{
	if (m_goldGainMultiplierIncreaseBase > 0.f || m_goldGainMultiplierIncreasePerLevel > 0.f)
	{
		outStr += StringUtils::StringF("Gold Gain: +%.2fx (+%.2fx Per Level)\n", m_goldGainMultiplierIncreaseBase, m_goldGainMultiplierIncreasePerLevel);
	}
	if (m_interestRateIncreaseBase > 0.f || m_interestRateIncreasePerLevel > 0.f)
	{
		outStr += StringUtils::StringF("Interest Rate: +%.2fx (+%.2fx Per Level)\n", m_interestRateIncreaseBase, m_interestRateIncreasePerLevel);
	}
	if (m_interestTimerSpeedIncreaseBase > 0.f || m_interestTimerSpeedIncreasePerLevel > 0.f)
	{
		outStr += StringUtils::StringF("Interest Timer Speed: +%.2fx (+%.2fx Per Level)\n", m_interestTimerSpeedIncreaseBase, m_interestTimerSpeedIncreasePerLevel);
	}
	if (m_baseSells > 0.f || m_sellsPerLevel > 0.f)
	{
		outStr += StringUtils::StringF("Sells: +%d (+%d Per Level)\n", m_baseSells, m_sellsPerLevel);
	}
	if (m_baseSellRateIncrease > 0.f || m_sellRateIncreasePerLevel > 0.f)
	{
		outStr += StringUtils::StringF("Sell Rate: +%.2fx (+%.2fx Per Level)\n", m_baseSellRateIncrease, m_sellRateIncreasePerLevel);
	}
	if (m_creditLimitIncreaseBase > 0.f || m_creditLimitIncreasePerLevel > 0.f)
	{
		outStr += StringUtils::StringF("Credit Limit: +%.2f (+%.2f Per Level)\n", m_creditLimitIncreaseBase, m_creditLimitIncreasePerLevel);
	}
	if (m_debtInterestRateDecreaseBase != 0.f || m_debtInterestRateDecreasePerLevel != 0.f)
	{
		outStr += StringUtils::StringF("Debt Interest Rate: +%.2fx (+%.2fx Per Level)\n", m_debtInterestRateDecreaseBase, m_debtInterestRateDecreasePerLevel);
	}	
}



//----------------------------------------------------------------------------------------------------------------------
EconomyRunModifier::EconomyRunModifier(EconomyRunModifierDef const& def) : RunModifier(def)
{

}



//----------------------------------------------------------------------------------------------------------------------
void EconomyRunModifier::Apply(SystemContext const& context) const
{
	SCRunData& scRunData = context.GetSingleton<SCRunData>();
	RunData& runData = *scRunData.m_data;

	runData.m_needsModifierRecalculation = true;
}



//----------------------------------------------------------------------------------------------------------------------
void EconomyRunModifier::ApplyToRunData(RunData& runData) const
{
	EconomyRunModifierDef const& def = GetDef();
	runData.m_goldGainMultiplier += def.m_goldGainMultiplierIncreaseBase + (def.m_goldGainMultiplierIncreasePerLevel * static_cast<float>(m_level - 1));
	runData.m_savingsInterestRate += def.m_interestRateIncreaseBase + (def.m_interestRateIncreasePerLevel * static_cast<float>(m_level - 1));
	runData.m_maxSellsPerMission += def.m_baseSells + (def.m_sellsPerLevel * (m_level - 1));
	runData.m_sellRefundRate += def.m_baseSellRateIncrease + (def.m_sellRateIncreasePerLevel * static_cast<float>(m_level - 1));
	runData.m_creditLimit += def.m_creditLimitIncreaseBase + (def.m_creditLimitIncreasePerLevel * static_cast<float>(m_level - 1));
	float interestRateMultiplier = 1.f - (def.m_interestRateIncreaseBase + (def.m_interestRateIncreasePerLevel * static_cast<float>(m_level - 1)));
	runData.m_interestTimerSeconds *= interestRateMultiplier;
	runData.m_debtInterestRate += def.m_debtInterestRateDecreaseBase + (def.m_debtInterestRateDecreasePerLevel * static_cast<float>(m_level - 1));
}



//----------------------------------------------------------------------------------------------------------------------
void EconomyRunModifier::GetDescription(std::string& outStr) const
{
	EconomyRunModifierDef const& def = GetDef();
	if (def.m_maxLevel > 1)
	{
		if (def.m_goldGainMultiplierIncreaseBase > 0.f || def.m_goldGainMultiplierIncreasePerLevel > 0.f)
		{
			float currentMultiplier = 1.f + def.m_goldGainMultiplierIncreaseBase + (def.m_goldGainMultiplierIncreasePerLevel * static_cast<float>(m_level - 1));
			float nextMultiplier = currentMultiplier + def.m_goldGainMultiplierIncreasePerLevel;
			outStr += StringUtils::StringF("Gold Gain: Current: %.2fx, Next: %.2fx\n", currentMultiplier, nextMultiplier);
		}
		if (def.m_interestRateIncreaseBase > 0.f || def.m_interestRateIncreasePerLevel > 0.f)
		{
			float currentMultiplier = def.m_interestRateIncreaseBase + (def.m_interestRateIncreasePerLevel * static_cast<float>(m_level - 1));
			float nextMultiplier = currentMultiplier + def.m_interestRateIncreasePerLevel;
			outStr += StringUtils::StringF("Interest Rate: Current: %.2fx, Next: %.2fx\n", currentMultiplier, nextMultiplier);
		}
		if (def.m_interestTimerSpeedIncreaseBase > 0.f || def.m_interestTimerSpeedIncreasePerLevel > 0.f)
		{
			float currentMultiplier = 1.f + def.m_interestTimerSpeedIncreaseBase + (def.m_interestTimerSpeedIncreasePerLevel * static_cast<float>(m_level - 1));
			float nextMultiplier = currentMultiplier + def.m_interestTimerSpeedIncreasePerLevel;
			outStr += StringUtils::StringF("Interest Timer Speed: Current: %.2fx, Next: %.2fx\n", currentMultiplier, nextMultiplier);
		}
		if (def.m_baseSells > 0 || def.m_sellsPerLevel > 0)
		{
			int currentSells = def.m_baseSells + (def.m_sellsPerLevel * (m_level - 1));
			int nextSells = currentSells + def.m_sellsPerLevel;
			outStr += StringUtils::StringF("Sells: Current: %d, Next: %d\n", currentSells, nextSells);
		}
		if (def.m_baseSellRateIncrease > 0.f || def.m_sellRateIncreasePerLevel > 0.f)
		{
			float currentMultiplier = def.m_baseSellRateIncrease + (def.m_sellRateIncreasePerLevel * static_cast<float>(m_level - 1));
			float nextMultiplier = currentMultiplier + def.m_sellRateIncreasePerLevel;
			outStr += StringUtils::StringF("Sell Rate: Current: %.2fx, Next: %.2fx\n", currentMultiplier, nextMultiplier);
		}
		if (def.m_creditLimitIncreaseBase > 0.f || def.m_creditLimitIncreasePerLevel > 0.f)
		{
			float currentMultiplier = def.m_creditLimitIncreaseBase + (def.m_creditLimitIncreasePerLevel * static_cast<float>(m_level - 1));
			float nextMultiplier = currentMultiplier + def.m_creditLimitIncreasePerLevel;
			outStr += StringUtils::StringF("Credit Limit: Current: %.2f, Next: %.2f\n", currentMultiplier, nextMultiplier);
		}
		if (def.m_debtInterestRateDecreaseBase != 0.f || def.m_debtInterestRateDecreasePerLevel != 0.f)
		{
			float currentMultiplier = def.m_debtInterestRateDecreaseBase + (def.m_debtInterestRateDecreasePerLevel * static_cast<float>(m_level - 1));
			float nextMultiplier = currentMultiplier + def.m_debtInterestRateDecreasePerLevel;
			outStr += StringUtils::StringF("Debt Interest Rate: Current: %.2fx, Next: %.2fx\n", currentMultiplier, nextMultiplier);
		}

		outStr += StringUtils::StringF("Level: %d/%d\n", m_level, m_def.m_maxLevel);
	}
	else
	{
		if (def.m_goldGainMultiplierIncreaseBase > 0.f || def.m_goldGainMultiplierIncreasePerLevel > 0.f)
		{
			outStr += StringUtils::StringF("Gold Gain: +%.2fx\n", def.m_goldGainMultiplierIncreaseBase);
		}
		if (def.m_interestRateIncreaseBase > 0.f || def.m_interestRateIncreasePerLevel > 0.f)
		{
			outStr += StringUtils::StringF("Interest Rate: +%.2fx\n", def.m_interestRateIncreaseBase);
		}
		if (def.m_interestTimerSpeedIncreaseBase > 0.f || def.m_interestTimerSpeedIncreasePerLevel > 0.f)
		{
			outStr += StringUtils::StringF("Interest Timer Speed: +%.2fx\n", def.m_interestTimerSpeedIncreaseBase);
		}
		if (def.m_baseSells > 0 || def.m_sellsPerLevel > 0)
		{
			outStr += StringUtils::StringF("Sells: +%d\n", def.m_baseSells);
		}
		if (def.m_baseSellRateIncrease > 0.f || def.m_sellRateIncreasePerLevel > 0.f)
		{
			outStr += StringUtils::StringF("Sell Rate: +%.2fx\n", def.m_baseSellRateIncrease);
		}
		if (def.m_creditLimitIncreaseBase > 0.f || def.m_creditLimitIncreasePerLevel > 0.f)
		{
			outStr += StringUtils::StringF("Credit Limit: +%.2f\n", def.m_creditLimitIncreaseBase);
		}
		if (def.m_debtInterestRateDecreaseBase != 0.f || def.m_debtInterestRateDecreasePerLevel != 0.f)
		{
			outStr += StringUtils::StringF("Debt Interest Rate: +%.2fx\n", def.m_debtInterestRateDecreaseBase);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
EconomyRunModifierDef const& EconomyRunModifier::GetDef() const
{
	return static_cast<EconomyRunModifierDef const&>(m_def);
}
