// Bradley Christensen - 2022-2026
#include "SCRunData.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/Noise.h"



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
bool IsRequirementMet(Name requirement, std::vector<RunModifier*> const& activeModifiers)
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
		if (runModifierDef->m_levelRequirement > levelData.m_level)
		{
			continue;
		}

		bool requirementsMet = true;
		for (Name requirement : runModifierDef->m_requirements)
		{
			if (!IsRequirementMet(requirement, m_activeRunModifiers))
			{
				requirementsMet = false;
				break;
			}
		}

		if (!requirementsMet)
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
