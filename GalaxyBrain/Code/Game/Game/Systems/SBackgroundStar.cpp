// Bradley Christensen - 2023
#include "SBackgroundStar.h"
#include "Game/Game/Singletons/SCUniverse.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Math/Noise.h"



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Startup()
{
	AddWriteDependencies<SCUniverse>();

	SCUniverse& univ = *g_ecs->GetComponent<SCUniverse>();
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Run(SystemContext const& context)
{
	// noise test?

	float result = 0;
	for (int i = 0; i < 10; ++i)
	{
		float noise = GetFractalNoise1D(i);
		result += noise;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Shutdown()
{

}
 