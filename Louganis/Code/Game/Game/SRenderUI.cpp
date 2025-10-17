// Bradley Christensen - 2022-2025
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
	AddReadDependencies<CHealth, CRender>();
	AddWriteDependencies<SCRender, Renderer>();

	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	scRender.m_uiVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderUI::Run(SystemContext const& context)
{
	SCRender& scRender = g_ecs->GetSingleton<SCRender>();
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
	auto& healthStorage = g_ecs->GetArrayStorage<CHealth>();

	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(scRender.m_uiVBO);
	vbo.ClearVerts();

	// Render Health bars
	for (auto it = g_ecs->Iterate<CHealth>(context); it.IsValid(); ++it)
	{
		CHealth const& health = healthStorage[it];

		if (health.GetNeverShowHealthBar())
		{
			continue;
		}

		if (health.m_currentHealth == health.m_maxHealth || health.m_maxHealth <= 0.f)
		{
			continue;
		}

		CRender const& render = renderStorage[it];

		float healthPercentage = health.m_currentHealth / health.m_maxHealth;

		Vec2 headLocation = render.GetRenderPosition() + Vec2(0.f, 0.55f * render.m_scale);
		float healthBarWidth = render.m_scale * 0.75f;
		AABB2 backgroundVerts;
		backgroundVerts.SetCenter(headLocation);
		backgroundVerts.SetDimsAboutCenter(Vec2(healthBarWidth, 0.0833f));
		AABB2 healthVerts = backgroundVerts;
		healthVerts.maxs.x = healthVerts.mins.x + healthBarWidth * healthPercentage;
		VertexUtils::AddVertsForAABB2(vbo, backgroundVerts, Rgba8::DarkGray);
		VertexUtils::AddVertsForAABB2(vbo, healthVerts, Rgba8::Crimson);
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