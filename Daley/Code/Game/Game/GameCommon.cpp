// Bradley Christensen - 2022-2026
#include "GameCommon.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
float StaticGameSettings::s_burnDecayK = -1.f * MathUtils::LogF(StaticGameSettings::s_burnDecayedValueAfterOneSecond);
float StaticGameSettings::s_oneOverBurnDecayK = 1.f / StaticGameSettings::s_burnDecayK;