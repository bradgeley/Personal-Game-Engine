// Bradley Christensen - 2022-2026
#include "SCRunData.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void RunData::Shutdown()
{

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