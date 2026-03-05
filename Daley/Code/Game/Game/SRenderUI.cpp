// Bradley Christensen - 2022-2026
#include "SRenderUI.h"
#include "CHealth.h"
#include "CRender.h"
#include "SCRender.h"
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
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Run(SystemContext const& context)
{
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();
	SCRender const& scRender = g_ecs->GetSingleton<SCRender>();

	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(scRender.m_uiVBO);
	vbo.ClearVerts();

	// Render Health bars
	for (auto it = g_ecs->Iterate<CRender, CHealth>(context); it.IsValid(); ++it)
	{
		CRender const& render = renderStorage[it];
		if (!render.GetIsInCameraView())
		{
			continue;
		}

		CHealth const& health = healthStorage[it];

		if (health.GetNeverShowHealthBar())
		{
			continue;
		}

		if (health.m_currentHealth == health.m_maxHealth || health.m_maxHealth <= 0.f)
		{
			continue;
		}

		Vec2 healthBarLocation = render.GetRenderPosition() + Vec2(0.f, 0.55f * render.m_renderRadius);
		float healthBarWidth = render.m_renderRadius * 0.75f;

		float healthPercentage = health.m_currentHealth / health.m_maxHealth;
		float burnSaturation = health.GetBurnSaturation();
		float poisonSaturation = health.GetPoisonSaturation();

		// Background
		AABB2 backgroundBox;
		backgroundBox.SetCenter(healthBarLocation);
		backgroundBox.SetDimsAboutCenter(Vec2(healthBarWidth, 0.0833f));
		VertexUtils::AddVertsForAABB2(vbo, backgroundBox, Rgba8::DarkGray);

		AABB2 healthBox = backgroundBox;
		healthBox.maxs.x = healthBox.mins.x + healthBarWidth * healthPercentage;

		VertexUtils::AddVertsForAABB2(vbo, healthBox, Rgba8::Crimson);
		if (burnSaturation > 0.f)
		{
			AABB2 burnBox = healthBox;
			burnBox.maxs.x = burnBox.mins.x + healthBarWidth * healthPercentage * burnSaturation;
			VertexUtils::AddVertsForAABB2(vbo, burnBox, Rgba8::Orange);
		}
		if (poisonSaturation > 0.f)
		{
			AABB2 poisonBox = healthBox;
			poisonBox.mins.x = poisonBox.maxs.x - healthBarWidth * healthPercentage * poisonSaturation;
			VertexUtils::AddVertsForAABB2(vbo, poisonBox, Rgba8::Green);
		}
	}

	g_renderer->SetModelMatrix(Mat44::Identity);
	g_renderer->BindTexture(nullptr);
	g_renderer->BindShader(nullptr);
	g_renderer->DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Shutdown()
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	g_renderer->ReleaseVertexBuffer(scRender.m_uiVBO);
}