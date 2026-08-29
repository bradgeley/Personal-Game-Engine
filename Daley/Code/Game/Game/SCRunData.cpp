// Bradley Christensen - 2022-2026
#include "SCRunData.h"



//----------------------------------------------------------------------------------------------------------------------
void RunData::Shutdown()
{

}



//----------------------------------------------------------------------------------------------------------------------
int RunData::CalculateCurrentLevel() const
{
    if (m_currentXP < 0.f)
    {
        return 0.f;
    }

    int currentLevel = 0;
	float currentXp = m_currentXP;
	float currentXpRequirement = StaticGameSettings::s_baseLevelXpRequirement;

    while (currentXp > currentXpRequirement)
    {
        currentLevel++;
        currentXp -= currentXpRequirement;
        currentXpRequirement *= StaticGameSettings::s_perLevelXpRequirement;
    }

    return currentLevel;
}
