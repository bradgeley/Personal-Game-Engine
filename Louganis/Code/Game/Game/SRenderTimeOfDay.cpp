// Bradley Christensen - 2022-2025
#include "SRenderTimeOfDay.h"
#include "CCamera.h"
#include "SCRender.h"
#include "SCTime.h"
#include "TimeOfDay.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/VertexBuffer.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderTimeOfDay::Startup()
{
	AddReadDependencies<SCTime, CCamera>();
	AddWriteDependencies<SCRender, Renderer>();

	auto& scRender = g_ecs->GetSingleton<SCRender>();
	scRender.m_timeOfDayVBO = g_renderer->MakeVertexBuffer<Vertex_PCU>();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderTimeOfDay::Run(SystemContext const& context)
{
	auto& scTime = g_ecs->GetSingleton<SCTime>();
	auto& scRender = g_ecs->GetSingleton<SCRender>();

	CCamera const* activeCamera = nullptr;
	for (auto it = g_ecs->Iterate<CCamera>(context); it.IsValid(); ++it)
	{
		CCamera const* camera = g_ecs->GetComponent<CCamera>(it);
		if (camera->m_isActive)
		{
			activeCamera = camera;
		}
	}

	if (!activeCamera)
	{
		return;
	}

	Rgba8 color = TimeOfDayUtils::GetTimeOfDayTint(scTime);

	VertexBuffer& vbo = *g_renderer->GetVertexBuffer(scRender.m_timeOfDayVBO);
	vbo.ClearVerts();
	VertexUtils::AddVertsForAABB2(vbo, activeCamera->m_camera.GetTranslatedOrthoBounds2D(), color);
	g_renderer->BindTexture(nullptr);
	g_renderer->BindShader(nullptr);
	g_renderer->SetModelConstants(ModelConstants());
	g_renderer->DrawVertexBuffer(vbo);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderTimeOfDay::Shutdown()
{
	auto& scRender = g_ecs->GetSingleton<SCRender>();
	g_renderer->ReleaseVertexBuffer(scRender.m_timeOfDayVBO);
}
