// Bradley Christensen - 2023
#include "SBackgroundStar.h"
#include "Game/SCUniverse.h"
#include "Game/CCamera.h"
#include "Game/CPhysics.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/Grid2D.h"
#include "Engine/Math/RandomNumberGenerator.h"



// Todo: name star "Sonali Ah La La"
constexpr int STAR_COUNT = 250;



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Startup()
{
	AddWriteDependencies<SCUniverse>();

	SCUniverse& univ = g_ecs->GetSingleton<SCUniverse>();

	for (int i = 0; i < STAR_COUNT; ++i)
	{
		Star star;
		star.m_pos = { g_rng->GetRandomFloatInRange(0.f, g_window->GetAspect()), g_rng->GetRandomFloatInRange(0.f, 1.f) };
		star.Randomize();
		univ.m_parallaxStars.emplace_back(star);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Run(SystemContext const& context)
{
	Camera renderCamera(Vec3(0.f, 0.f, 0.f), Vec3(g_window->GetAspect(), 1.f, 1000.f));
	g_renderer->BeginCameraAndWindow(&renderCamera, g_window);

	// Render portion
	g_renderer->ClearDepth(1.f);
	g_renderer->ClearScreen(Rgba8::Black);

	SCUniverse& univ = *g_ecs->GetComponent<SCUniverse>();

	// Should happen only once
	for (auto it = g_ecs->Iterate<CCamera, CPhysics>(context); it.IsValid(); ++it)
	{
		CCamera* camera = g_ecs->GetComponent<CCamera>(it.m_currentIndex);
		CPhysics* phys = g_ecs->GetComponent<CPhysics>(it.m_currentIndex);

		Vec2 starVelocity = -0.1f * phys->m_velocity.GetNormalized();
		for (auto& star : univ.m_parallaxStars)
		{
			star.m_pos += (1.f / star.m_distance) * starVelocity;

			bool wentOffScreen = true;
			if (star.m_pos.x > g_window->GetAspect()) star.m_pos.x -= g_window->GetAspect();
			else if (star.m_pos.x < 0.f) star.m_pos.x += g_window->GetAspect();
			else if (star.m_pos.y > 1.f) star.m_pos.y -= 1.f;
			else if (star.m_pos.y < 0.f) star.m_pos.y += 1.f;
			else wentOffScreen = false;

			if (wentOffScreen)
			{
				star.Randomize();
			}
		}

		auto bounds = camera->m_camera.GetOrthoBounds2D();
		auto dims = bounds.GetDimensions();

		VertexBuffer starsBuffer;
		for (auto& star : univ.m_parallaxStars)
		{
			float size = 1.f / star.m_distance;
			AddVertsForDisc2D(starsBuffer.GetMutableVerts(), star.m_pos, size, 16, star.m_tint);
		}

		g_renderer->BindShader(0);
		g_renderer->BindTexture(0);
		g_renderer->DrawVertexBuffer(&starsBuffer);
	}

	g_renderer->EndCameraAndWindow(&renderCamera, g_window);
}



//----------------------------------------------------------------------------------------------------------------------
void SBackgroundStar::Shutdown()
{

}
 