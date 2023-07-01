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

		AABB2 cameraBounds = camera->m_camera.GetOrthoBounds2D();
		Vec2 cameraDims = cameraBounds.GetDimensions();

		Grid2D<float> noiseValues((int) cameraDims.x, (int) cameraDims.y, 0.f);
		for (int x = 0; x < (int) cameraDims.x; ++x)
		{
			for (int y = 0; y < (int) cameraDims.y; ++y)
			{
				float noise = GetPerlinNoise2D(x, y, STAR_NOISE_SCALE, 1, 0.5f, 2.f, false);
				noiseValues.Set(x, y, noise);
			}
		}

		for (int x = 0; x < (int) cameraDims.x; ++x)
		{
			for (int y = 0; y < (int) cameraDims.y; ++y)
			{
				float noise = noiseValues.Get(x, y);

				int indexNorth = noiseValues.GetIndexForCoords(x, y + 1);
				if (noiseValues.IsValidIndex(indexNorth))
				{
					float noiseNorth = noiseValues.Get(indexNorth);
					if (noiseNorth > noise)
					{
						continue;
					}
				}

				int indexSouth = noiseValues.GetIndexForCoords(x, y - 1);
				if (noiseValues.IsValidIndex(indexSouth))
				{
					float noiseSouth = noiseValues.Get(indexSouth);
					if (noiseSouth > noise)
					{
						continue;
					}
				}

				int indexEast = noiseValues.GetIndexForCoords(x + 1, y);
				if (noiseValues.IsValidIndex(indexEast))
				{
					float noiseEast = noiseValues.Get(indexEast);
					if (noiseEast > noise)
					{
						continue;
					}
				}

				int indexWest = noiseValues.GetIndexForCoords(x - 1, y);
				if (noiseValues.IsValidIndex(indexWest))
				{
					float noiseWest = noiseValues.Get(indexWest);
					if (noiseWest > noise)
					{
						continue;
					}
				}

				// This x, y is a local maxima - draw a star
				Star newStar;
				newStar.m_pos = cameraBounds.mins + Vec2(x, y);
				newStar.m_size = 1.f / camera->m_zoom;
				univ.m_parallaxStarsClose.emplace_back(newStar);
			}
		}
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
 