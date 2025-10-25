// Bradley Christensen - 2022-2025
#include "SRenderStorm.h"
#include "SCStorm.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"



//----------------------------------------------------------------------------------------------------------------------
void SRenderStorm::Startup()
{
	AddReadDependencies<SCStorm>();
	AddWriteDependencies<Renderer>();

	constexpr int numSides = 1024;
	constexpr int numVerts = numSides * 3;
	SCStorm& scStorm = g_ecs->GetSingleton<SCStorm>();
	scStorm.m_stormCircleVertexBuffer = g_renderer->MakeVertexBuffer<Vertex_PCU>(numVerts);
	
	VertexUtils::AddVertsForDisc2D(
		*g_renderer->GetVertexBuffer(scStorm.m_stormCircleVertexBuffer),
		Vec2::ZeroVector,
		1.f,
		1024,
		Rgba8::White,
		AABB2::ZeroToOne
	);
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderStorm::Run(SystemContext const& context)
{
	SCStorm const& scStorm = g_ecs->GetSingleton<SCStorm>();
	VertexBuffer* stormCircleVBO = g_renderer->GetVertexBuffer(scStorm.m_stormCircleVertexBuffer);

	ModelConstants stormConstants;
	stormConstants.m_modelMatrix = Mat44::CreateTranslation3D(scStorm.m_stormLocation.x, scStorm.m_stormLocation.y, 0.f);
	stormConstants.m_modelMatrix.AppendUniformScale2D(scStorm.m_stormRadius);
	Rgba8 stormTint = Rgba8::DarkViolet;
	stormTint.GetAsFloats(stormConstants.m_modelRgba);
	stormConstants.m_modelRgba[3] = 0.25f;

	g_renderer->SetBlendMode(BlendMode::Alpha);
	g_renderer->SetModelConstants(stormConstants);
	g_renderer->BindTexture(nullptr); // No texture
	g_renderer->BindShader(nullptr);  // No shader (for now)
	g_renderer->DrawVertexBuffer(*stormCircleVBO);

	g_renderer->ResetModelConstants();
}



//----------------------------------------------------------------------------------------------------------------------
void SRenderStorm::Shutdown()
{
	SCStorm& scStorm = g_ecs->GetSingleton<SCStorm>();
	g_renderer->ReleaseVertexBuffer(scStorm.m_stormCircleVertexBuffer);
}
