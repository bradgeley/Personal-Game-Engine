// Bradley Christensen - 2022-2025
#include "SLighting.h"
#include "SCRender.h"
#include "SCTime.h"
#include "SCWorld.h"
#include "WorldShaderCPU.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/ConstantBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Startup()
{
    AddReadDependencies<SCTime, SCWorld>();
	AddWriteDependencies<SCRender, Renderer>();

    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    scRender.m_lightingConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(LightingConstants));
}



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Run(SystemContext const&)
{
    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    SCTime& scTime = g_ecs->GetSingleton<SCTime>();
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();

	Rgba8 timeOfDayTint = scWorld.m_worldSettings.m_timeOfDayTints[(size_t) scTime.m_timeOfDay];
    TimeOfDay nextTimeOfDay = (TimeOfDay) MathUtils::IncrementIntInRange((int) scTime.m_timeOfDay, 0, (int) TimeOfDay::Count - 1, true);
	Rgba8 nextTimeOfDayTint = scWorld.m_worldSettings.m_timeOfDayTints[(size_t) nextTimeOfDay];
	Rgba8 outdoorLightTint = Rgba8::Lerp(timeOfDayTint, nextTimeOfDayTint, scTime.m_isTransitioning ? scTime.m_dayTimer.GetElapsedFraction() : 0.f);

	LightingConstants lightingConstants;
    Rgba8::White.GetAsFloats(lightingConstants.m_ambientLightTint);
	outdoorLightTint.GetAsFloats(lightingConstants.m_outdoorLightTint);
	Rgba8::LightOrange.GetAsFloats(lightingConstants.m_indoorLightTint);
    lightingConstants.m_ambientLightIntensity = 0.01f; // 0 = pitch black, 1 = full brightness

    ConstantBuffer* lightingCbo = g_renderer->GetConstantBuffer(scRender.m_lightingConstantsBuffer);
    lightingCbo->Update(lightingConstants);
}



//----------------------------------------------------------------------------------------------------------------------
void SLighting::Shutdown()
{
    SCRender& scRender = g_ecs->GetSingleton<SCRender>();
    g_renderer->ReleaseConstantBuffer(scRender.m_lightingConstantsBuffer);
}
