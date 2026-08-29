// Bradley Christensen - 2022-2026
#include "GameCommon.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
float StaticGameSettings::s_burnDecayK = -1.f * MathUtils::LogF(StaticGameSettings::s_burnDecayedValueAfterOneSecond);
float StaticGameSettings::s_oneOverBurnDecayK = 1.f / StaticGameSettings::s_burnDecayK;

// Formula: B * (m^n - m) / (m - 1) where B = base level exp requirement, m = per level xp requirement, n = max level
uint64_t StaticGameSettings::s_maximumExperience = static_cast<uint64_t>(StaticGameSettings::s_baseLevelExpRequirement * (MathUtils::PowD(StaticGameSettings::s_expRequirementExponent, StaticGameSettings::s_maxLevel - 1) - 1.0) / (StaticGameSettings::s_expRequirementExponent - 1.0));



//----------------------------------------------------------------------------------------------------------------------
std::array<uint64_t, StaticGameSettings::s_maxLevel>& StaticGameSettings::GetExpLookupTable()
{
	static std::array<uint64_t, StaticGameSettings::s_maxLevel> s_totalExperienceRequiredForLevel = []{
		std::array<uint64_t, StaticGameSettings::s_maxLevel> table = {};
		table[0] = 0;
		for (int level = 1; level < StaticGameSettings::s_maxLevel; ++level)
		{
			table[level] = static_cast<uint64_t>(StaticGameSettings::s_baseLevelExpRequirement * (MathUtils::PowD(StaticGameSettings::s_expRequirementExponent, level) - 1.0) / (StaticGameSettings::s_expRequirementExponent - 1.0));
		}
		return table;
	}();
	return s_totalExperienceRequiredForLevel;
}
