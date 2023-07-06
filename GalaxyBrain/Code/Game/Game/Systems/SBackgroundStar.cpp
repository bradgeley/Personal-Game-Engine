// Bradley Christensen - 2023
#include "SBackgroundStar.h"
#include "Game/Game/Singletons/SCUniverse.h"
#include "Game/Game/Components/CCamera.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/Grid2D.h"



constexpr float STAR_NOISE_SCALE = 10.f;



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Startup()
{
	AddWriteDependencies<SCUniverse>();

}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Run(SystemContext const& context)
{
	g_renderer->BeginWindow(g_window);
	g_renderer->ClearDepth(1.f);
	g_renderer->ClearScreen(Rgba8::Black);

	SCUniverse& univ = *g_ecs->GetComponent<SCUniverse>();
	univ.m_parallaxStarsClose.clear();

	for (auto it = g_ecs->Iterate<CCamera>(context); it.IsValid(); ++it)
	{
		CCamera* camera = g_ecs->GetComponent<CCamera>(it.m_currentIndex);

		
	}

	VertexBuffer starsBuffer;
	for (auto& star : univ.m_parallaxStarsClose)
	{
		AABB2 starBounds;
		starBounds.mins = star.m_pos;
		starBounds.maxs = star.m_pos + Vec2(star.m_size, star.m_size);
		AddVertsForAABB2(starsBuffer.GetMutableVerts(), starBounds);
	}

	g_renderer->BindShader(0);
	g_renderer->BindTexture(0);
	g_renderer->DrawVertexBuffer(&starsBuffer);
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Shutdown()
{

}
 