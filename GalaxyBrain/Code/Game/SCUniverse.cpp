// Bradley Christensen - 2023
#include "SCUniverse.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void Star::Randomize()
{
	m_distance = g_rng->GetRandomFloatInRange(750.f, 1000.f);
	float alphaf = RangeMap(m_distance, 0.f, 1000.f, 50.f, 255.f);
	alphaf += g_rng->GetRandomFloatInRange(0.f, 150.f);
	m_tint = Rgba8(g_rng->GetRandomIntInRange(200, 255), g_rng->GetRandomIntInRange(150, 255), g_rng->GetRandomIntInRange(200, 255), (uint8_t) alphaf);
}
