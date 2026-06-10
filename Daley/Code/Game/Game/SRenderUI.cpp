// Bradley Christensen - 2022-2026
#include "SRenderUI.h"
#include "CHealth.h"
#include "CRender.h"
#include "CTime.h"
#include "HealthBarShaderCPU.h"
#include "SCRender.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/ShaderAsset.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/InstanceBuffer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Startup()
{
	AddReadDependencies<CHealth, CRender, SCRender>();
	AddWriteDependencies<Renderer>();

	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	scRender.m_uiVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();

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
void SRenderUI::Run(SystemContext const& context)
{
	// Read Dependencies
	auto const& renderStorage = g_ecs->GetArrayStorage<CRender>();
	auto const& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	SCRender const& scRender = g_ecs->GetSingleton<SCRender>();

	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(scRender.m_uiVBO);
	vbo.ClearVerts();

	InstanceBuffer& healthBarIBO = *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer);
	healthBarIBO.ClearInstances();

	g_renderer->BindConstantBuffer(scRender.m_healthBarConstantsBuffer, 5);

	ShaderAsset* healthBarShaderAsset = g_assetManager->Get<ShaderAsset>(scRender.m_healthBarShaderAsset);
	if (!healthBarShaderAsset)
	{
		return;
	}

	// Render Health bars
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

		CTime* time = g_ecs->GetComponent<CTime>(it);
		if (time && time->IsSlowed())
		{
			// Slow is the only status effect rn
			Vec2 slowStatusLocation = render.GetRenderPosition() + Vec2(0.f, 0.55f * render.m_renderRadius) + Vec2(-0.75f, -0.0f);
			VertexUtils::AddVertsForDisc2D(vbo, slowStatusLocation, 0.1f, 6, Rgba8(80, 80, 80, 255));
		}
	}

	g_renderer->SetModelMatrix(Mat44::Identity);
	g_renderer->BindTexture(nullptr);
	g_renderer->BindShader(nullptr);
	g_renderer->DrawVertexBuffer(vbo);

	g_renderer->BindShader(healthBarShaderAsset->GetShaderID());
	g_renderer->DrawInstanced(6, *g_renderer->GetInstanceBuffer(scRender.m_healthBarInstanceBuffer));
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Shutdown()
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	g_renderer->ReleaseVertexBuffer(scRender.m_uiVBO);
}