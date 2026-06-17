// Bradley Christensen - 2022-2026
#include "SRenderHealthBars.h"
#include "CHealth.h"
#include "CRender.h"
#include "HealthBarShaderCPU.h"
#include "SCAssetManager.h"
#include "SCRender.h"
#include "SCRenderer.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Startup()
{
	AddReadDependencies<CHealth, CRender, SCRender>();
	AddWriteDependencies<SCAssetManager, SCRenderer>();

	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	Renderer* renderer = g_ecs->GetSingleton<SCRenderer>().m_renderer;
	AssetManager* assetManager = g_ecs->GetSingleton<SCAssetManager>().m_assetManager;

	scRender.m_healthBarConstantsBuffer = renderer->MakeConstantBuffer(sizeof(HealthBarRenderConstants));
	scRender.m_healthBarInstanceBuffer = renderer->MakeInstanceBuffer();

	InstanceBuffer& healthBarIBO = *renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	healthBarIBO.Initialize<HealthBarInstance>();

	HealthBarRenderConstants constants;
	Rgba8::DarkGray.GetAsFloats(constants.m_backgroundTint);
	Rgba8::Crimson.GetAsFloats(constants.m_healthTint);
	Rgba8::Green.GetAsFloats(constants.m_poisonTint);
	Rgba8::Orange.GetAsFloats(constants.m_fireTint);

	ConstantBuffer& healthBarCBO = *renderer->GetConstantBuffer(scRender.m_healthBarConstantsBuffer);
	healthBarCBO.Update(constants);

	scRender.m_healthBarShaderAsset = assetManager->AsyncLoad<ShaderAsset>("Data/Shaders/HealthBarShader.xml");
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Shutdown() const
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	Renderer* renderer = g_ecs->GetSingleton<SCRenderer>().m_renderer;
	AssetManager* assetManager = g_ecs->GetSingleton<SCAssetManager>().m_assetManager;

	renderer->ReleaseConstantBuffer(scRender.m_healthBarConstantsBuffer);
	renderer->ReleaseInstanceBuffer(scRender.m_healthBarInstanceBuffer);

	assetManager->Release(scRender.m_healthBarShaderAsset);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderHealthBars::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& renderStorage = context.GetArrayStorageConst<CRender>();
	auto& healthStorage = context.GetArrayStorageConst<CHealth>();
	SCRender const& scRender = context.GetSingletonConst<SCRender>();

	// Write Dependencies
	AssetManager* assetManager = context.GetSingleton<SCAssetManager>().m_assetManager;
	Renderer* renderer = context.GetSingleton<SCRenderer>().m_renderer;

	ShaderAsset const* healthBarShaderAsset = assetManager->Get<ShaderAsset>(scRender.m_healthBarShaderAsset);

	InstanceBuffer& healthBarIBO = *renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	healthBarIBO.ClearInstances();

	for (auto it = context.Iterate<CRender, CHealth>(); it.IsValid(); ++it)
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
		renderer->BindConstantBuffer(scRender.m_healthBarConstantsBuffer, 5);
		renderer->BindShader(healthBarShaderAsset->GetShaderID());
		renderer->DrawInstanced(6, *renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer));
	}
}