// Bradley Christensen - 2022-2026
#include "SCRunData.h"
#include "TowerPlacementRequest.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
void RunData::InitAttributes()
{
	ResetModifiableAttributes();
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::Shutdown()
{
	m_runModifierPool.Shutdown();
	for (RunModifier* activeModifier : m_activeRunModifiers)
	{
		delete activeModifier;
	}
	m_activeRunModifiers.clear();
}



//----------------------------------------------------------------------------------------------------------------------
ExperienceLevelData RunData::GetLevelData(uint64_t experience)
{
	auto const& expLookupTable = StaticGameSettings::GetExpLookupTable();

	auto it = std::upper_bound(expLookupTable.begin(), expLookupTable.end(), experience);

	ASSERT_OR_DIE(it != expLookupTable.begin(), "Experience is less than the first element in the exp lookup table, which is invalid");

	if (it == expLookupTable.end())
	{
		ExperienceLevelData levelData;
		levelData.m_isMaxLevel = true;
		levelData.m_level = StaticGameSettings::s_maxLevel;
		levelData.m_percentIntoLevel = 1.f;
		levelData.m_experienceIntoLevel = 0;
		levelData.m_experienceNeededForNextLevel = 0;
		levelData.m_totalExperienceForNextLevel = 0;
		return levelData;
	}

	uint64_t expRequiredForCurrentLevel = *(it - 1);
	uint64_t expRequiredForNextLevel = *it;

	ExperienceLevelData levelData;
	levelData.m_isMaxLevel = false;
	levelData.m_level = static_cast<int>(std::distance(expLookupTable.begin(), it));
	levelData.m_experienceIntoLevel = experience - expRequiredForCurrentLevel;
	levelData.m_experienceNeededForNextLevel = expRequiredForNextLevel - experience;
	levelData.m_totalExperienceForNextLevel = expRequiredForNextLevel - expRequiredForCurrentLevel;
	levelData.m_percentIntoLevel = static_cast<float>(static_cast<double>(levelData.m_experienceIntoLevel) / static_cast<double>(levelData.m_totalExperienceForNextLevel));
	return levelData;
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::GenerateModifierChoices()
{
	std::vector<RunModifierDef const*> const& allRunModifiers = m_runModifierPool.m_runModifierDefs;

	std::vector<RunModifierDef const*> filteredModifiers;

	for (RunModifierDef const*& modifierChoice : m_modifierChoices)
	{
		modifierChoice = nullptr;
	}

	ExperienceLevelData levelData = GetLevelData(m_experience);

	// Filter out modifiers that are already active and at max level, or require a higher level
	float combinedWeight = 0.f;
	for (RunModifierDef const* runModifierDef : allRunModifiers)
	{
		if (!runModifierDef->AreRequirementsMet(*this))
		{
			continue;
		}

		bool isAlreadyActiveAndMaxLevel = false;
		for (RunModifier const* activeModifier : m_activeRunModifiers)
		{
			if (&activeModifier->m_def == runModifierDef && activeModifier->m_level == runModifierDef->m_maxLevel)
			{
				isAlreadyActiveAndMaxLevel = true;
				break;
			}
		}
		if (!isAlreadyActiveAndMaxLevel)
		{
			filteredModifiers.push_back(runModifierDef);
			combinedWeight += runModifierDef->m_weight;
		}
	}

	m_numActiveModifierChoices = 0;
	for (int choiceIndex = 0; choiceIndex < MAX_MODIFIER_CHOICES; ++choiceIndex)
	{
		float randomValue = Noise::GetNoiseZeroToOne1D(choiceIndex, m_seed + m_numModifierChoicesCompleted);
		randomValue = MathUtils::RangeMapClamped(randomValue, 0.f, 1.f, 0.f, combinedWeight);
		for (int filteredModIndex = 0; filteredModIndex < static_cast<int>(filteredModifiers.size()); ++filteredModIndex)
		{
			RunModifierDef const* runModifierDef = filteredModifiers[filteredModIndex];

			randomValue -= runModifierDef->m_weight;
			if (randomValue <= 0.f)
			{
				m_modifierChoices[choiceIndex] = runModifierDef;
				m_numActiveModifierChoices++;
				filteredModifiers[filteredModIndex] = filteredModifiers.back();
				filteredModifiers.pop_back();
				combinedWeight -= runModifierDef->m_weight;
				break;
			}
		}
	}

	if (m_numActiveModifierChoices == 0)
	{
		// For now, just remove any extra choices after all options are exhausted
		m_numModifierChoicesRemaining = 0;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::ResetModifiableAttributes()
{
	AttributeSet& attrs = m_attributes;

	attrs.SetValue("MaxHealth", StaticGameSettings::s_basePlayerHealth);
	attrs.SetValue("HealthRegen", StaticGameSettings::s_basePlayerHealthRegen);
	attrs.SetValue("Income", StaticGameSettings::s_baseIncome);
	attrs.SetValue("IncomeInterval", StaticGameSettings::s_baseIncomeInterval);
	attrs.SetValue("FlavorLimit", static_cast<float>(StaticGameSettings::s_flavorUnlockLimit));
	attrs.SetValue("SwirlLimit", static_cast<float>(StaticGameSettings::s_baseSwirlLimit));
	attrs.SetValue("SellLimit", static_cast<float>(StaticGameSettings::s_baseSellCount));
	attrs.SetValue("SellRefundRatio", StaticGameSettings::s_baseSellRefundRatio);
	attrs.SetValue("CreditLimit", StaticGameSettings::s_baseCreditLimit);
	attrs.SetValue("SavingsInterestRate", StaticGameSettings::s_baseSavingsInterestRate);
	attrs.SetValue("DebtInterestRate", StaticGameSettings::s_baseDebtInterestRate);
	attrs.SetValue("TowerCostRamp", StaticGameSettings::s_baseCostRampPerTower);
	attrs.SetValue("ExpMulti", 0.f);
	attrs.SetValue("GoldMulti", 0.f);
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::PrepareForMissionStart()
{
	m_attributes.SetValue("Gold", StaticGameSettings::s_baseGold * (1.f + m_attributes.GetValue("GoldMulti")));
	m_attributes.SetValue("Health", m_attributes.GetValue("MaxHealth", StaticGameSettings::s_basePlayerHealth));
	m_attributes.SetValue("SellsUsed", 0.f);

	m_incomeIntervalSecondsRemaining = m_attributes.GetValue("IncomeInterval");

	m_numActiveModifierChoices = 0;
	m_modifierChoices.fill(nullptr);

	m_needsModifierRecalculation = true;
	m_wantsModifierChoice = true;

	for (PlaceableTower& tower : m_placeableTowers)
	{
		tower.m_cost = tower.m_baseCost;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::OnTowerPlacementSuccess(TowerPlacementRequest const& request)
{
	m_attributes.AddValue("Gold", -request.m_cost);

	float costRampValue = m_attributes.GetValue("TowerCostRamp", 0.f);

	for (PlaceableTower& tower : m_placeableTowers)
	{
		if (tower.m_towerName == request.m_towerEntityName && tower.m_flavorName == request.m_flavorName)
		{
			tower.m_cost *= (1.f + costRampValue);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::OnSwirlSuccess(TowerSwirlRequest const& request)
{
	m_attributes.AddValue("Gold", -request.m_cost);
	
	float costRampValue = m_attributes.GetValue("TowerCostRamp", 0.f);

	for (PlaceableTower& tower : m_placeableTowers)
	{
		if (tower.m_flavorName == request.m_flavor)
		{
			tower.m_cost *= (1.f + costRampValue);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
float RunData::GetHealth() const
{
	return m_attributes.GetValue("Health", 0.f);
}



//----------------------------------------------------------------------------------------------------------------------
float RunData::GetAttribute(Name attribute, float defaultValue, bool errorIfNotFound) const
{
	return m_attributes.GetValue(attribute, defaultValue, errorIfNotFound);
}



//----------------------------------------------------------------------------------------------------------------------
int RunData::GetIntAttribute(Name attribute, int defaultValue, bool errorIfNotFound) const
{
	return static_cast<int>(m_attributes.GetValue(attribute, static_cast<float>(defaultValue), errorIfNotFound));
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::AddGold(float gold)
{
	float goldGainMulti = m_attributes.GetValue("GoldMulti", 0.f);
	m_attributes.AddValue("Gold", gold * (1.f + goldGainMulti));
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::AddSells(int sells)
{
	m_attributes.AddValue("SellsUsed", static_cast<float>(sells));
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::ApplyPlayerDamage(float damage)
{
	m_attributes.AddValue("Health", -damage);
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::ApplyPlayerHealthRegen(float deltaSeconds)
{
	float health = m_attributes.GetValue("Health", 0.f);
	if (health <= 0.f)
	{
		return;
	}

	float healthRegen = m_attributes.GetValue("HealthRegen", 0.f);
	float maxHealth = m_attributes.GetValue("MaxHealth", 0.f);

	health += healthRegen * deltaSeconds;
	if (health > maxHealth)
	{
		health = maxHealth;
	}

	m_attributes.SetValue("Health", health);
}



//----------------------------------------------------------------------------------------------------------------------
void RunData::ApplyPlayerIncome()
{
	while (m_incomeIntervalSecondsRemaining <= 0.f)
	{
		float income = m_attributes.GetValue("Income", StaticGameSettings::s_baseIncome);
		float incomeIncrease = GetLevelData(m_experience).m_level * StaticGameSettings::s_baseIncomePerLevel;

		float totalIncome = income + incomeIncrease;
		float gold = m_attributes.GetValue("Gold", 0.f);
		float goldMulti = m_attributes.GetValue("GoldMulti", 0.f);

		gold += totalIncome * (1.f + goldMulti);

		if (gold > 0.f)
		{
			float savingsInterestRate = m_attributes.GetValue("SavingsInterestRate", StaticGameSettings::s_baseSavingsInterestRate);
			gold += gold * savingsInterestRate;
		}
		else if (gold < 0.f)
		{
			float debtInterestRate = m_attributes.GetValue("DebtInterestRate", StaticGameSettings::s_baseDebtInterestRate);
			gold += gold * debtInterestRate;
		}

		m_attributes.SetValue("Gold", gold);

		float incomeInterval = m_attributes.GetValue("IncomeInterval", StaticGameSettings::s_baseIncomeInterval);
		m_incomeIntervalSecondsRemaining += incomeInterval;
	}
}



//----------------------------------------------------------------------------------------------------------------------
float RunData::ApplySell(float costOfPurchase)
{
	float refund = costOfPurchase * m_attributes.GetValue("SellRefundRatio", 0.f);
	m_attributes.AddValue("Gold", refund);
	return refund;
}
