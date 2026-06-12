// Bradley Christensen - 2022-2026
#include "SRenderHealthBars.h"
#include "CHealth.h"
#include "CRender.h"
#include "HealthBarShaderCPU.h"
#include "SCRender.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Startup()
{
	AddReadDependencies<CHealth, CRender, SCRender>();
	AddWriteDependencies<Renderer>();

	SCRender& scRender = g_ecs->GetSingleton<SCRender>();

	scRender.m_healthBarConstantsBuffer = g_renderer->MakeConstantBuffer(sizeof(HealthBarRenderConstants));
	scRender.m_healthBarInstanceBuffer = g_renderer->MakeInstanceBuffer();

	InstanceBuffer& healthBarIBO = *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	healthBarIBO.Initialize<HealthBarInstance>();

	HealthBarRenderConstants constants;
	Rgba8::DarkGray.GetAsFloats(constants.m_backgroundTint);
	Rgba8::Crimson.GetAsFloats(constants.m_healthTint);
	Rgba8::Green.GetAsFloats(constants.m_poisonTint);
	Rgba8::Orange.GetAsFloats(constants.m_fireTint);

	ConstantBuffer& healthBarCBO = *g_renderer->GetConstantBuffer(scRender.m_healthBarConstantsBuffer);
	healthBarCBO.Update(constants);

	scRender.m_healthBarShaderAsset = g_assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/HealthBarShader.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Run(SystemContext const& context)
{
	// Read Dependencies
	auto const& renderStorage = g_ecs->GetArrayStorage<CRender>();
	auto const& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	SCRender const& scRender = g_ecs->GetSingleton<SCRender>();

	ShaderAsset* healthBarShaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_healthBarShaderAsset);

	InstanceBuffer& healthBarIBO = *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	healthBarIBO.ClearInstances();

	for (auto it = g_ecs->Iterate<CRender, CHealth>(context); it.IsValid(); ++it)
	{
		CRender const& render = renderStorage[it];
		if (!render.GetIsInCameraView())
		{
			continue;
		}

		CHealth const& health = healthStorage[it];
		bool shouldShowHealthBar = !health.GetNeverShowHealthBar() && (health.m_currentHealth < health.m_maxHealth || health.GetBurnSaturation() > 0.f || health.GetPoisonSaturation() > 0.f);

		if (shouldShowHealthBar)
		{
			HealthBarInstance instance;
			instance.m_position = render.GetRenderPosition() + Vec2(0.f, 0.55f * render.m_renderRadius);
			instance.m_dimensions.x = 0.75f * render.m_renderRadius;
			instance.m_dimensions.y = 0.0833f;
			instance.m_fireFraction = health.GetBurnSaturation();
			instance.m_poisonFraction = health.GetPoisonSaturation();
			instance.m_healthFraction = health.GetHealthFraction();
			healthBarIBO.AddInstance(instance);
		}
	}

	if (healthBarShaderAsset)
	{
		g_renderer->BindConstantBuffer(scRender.m_healthBarConstantsBuffer, 5);
		g_renderer->BindShader(healthBarShaderAsset->GetShaderID());
		g_renderer->DrawInstanced(6, *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer));
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Shutdown()
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	g_renderer->ReleaseInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	g_renderer->ReleaseConstantBuffer(scRender.m_healthBarConstantsBuffer);
	g_assetManager->Release(scRender.m_healthBarShaderAsset);
}