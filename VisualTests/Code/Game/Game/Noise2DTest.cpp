// Bradley Christensen - 2023
#include "Noise2DTest.h"
#include "Engine/Math/Noise.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Font.h"



const IntVec2 DIMS = IntVec2{ 100, 100 };
const Vec2 GRID_SIZE = Vec2{ 20.f, 20.f };
constexpr float SCALE = 20.f;



//----------------------------------------------------------------------------------------------------------------------
void Noise2DTest::Startup()
{
	m_camera.SetOrthoBounds(Vec3(0.f, 0.f, 0.f), Vec3(100.f, 50.f, 1.f));

	m_seed = 0;

	GenerateNoise(m_seed); 
}



//----------------------------------------------------------------------------------------------------------------------
void Noise2DTest::Update(float deltaSeconds)
{
	static float accumulated = 0.f;

	accumulated += deltaSeconds;
	if (accumulated > 5.f)
	{
		accumulated = 0.f;
		m_seed++;

		GenerateNoise(m_seed);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Noise2DTest::Render() const
{
	g_renderer->BeginWindow(g_window);
	g_renderer->BeginCamera(&m_camera);
	g_renderer->ClearScreen(Rgba8::DarkGray);

	// Set up text rendering
	auto font = g_renderer->GetDefaultFont();

	VertexBuffer textVerts;
	VertexBuffer noiseVerts;

	// Raw noise
	Vec2 offset = Vec2(15.f, 12.5f);
	AABB2 rawNoiseBounds = AABB2(offset + Vec2(0.f, -GRID_SIZE.y / 2), offset + Vec2(GRID_SIZE.x, GRID_SIZE.y / 2));
	font->AddVertsForAlignedText2D(textVerts.GetMutableVerts(), Vec2(offset.x - 15.f, offset.y), Vec2(1.f, 0.f), 3.f, "Raw Noise", Rgba8::White);
	AddVertsForGrid2D(noiseVerts.GetMutableVerts(), rawNoiseBounds, (Rgba8*) m_rawNoise2D.GetRawData(), m_rawNoise2D.GetDimensions());

	// Fractal noise
	offset = Vec2(15.f, 37.5f);
	AABB2 fractalNoiseBounds = AABB2(offset + Vec2(0.f, -GRID_SIZE.y / 2), offset + Vec2(GRID_SIZE.x, GRID_SIZE.y / 2));
	font->AddVertsForAlignedText2D(textVerts.GetMutableVerts(), Vec2(offset.x - 15.f, offset.y), Vec2(1.f, 0.f), 3.f, "Fractal Noise", Rgba8::White);
	AddVertsForGrid2D(noiseVerts.GetMutableVerts(), fractalNoiseBounds, (Rgba8*) m_fractalNoise2D.GetRawData(), m_fractalNoise2D.GetDimensions(), Rgba8::Empty);

	// Perlin noise
	offset = Vec2(55.f, 12.5f);
	AABB2 perlinNoiseBounds = AABB2(offset + Vec2(0.f, -GRID_SIZE.y / 2), offset + Vec2(GRID_SIZE.x, GRID_SIZE.y / 2));
	font->AddVertsForAlignedText2D(textVerts.GetMutableVerts(), Vec2(offset.x - 15.f, offset.y), Vec2(1.f, 0.f), 3.f, "Perlin Noise", Rgba8::White);
	AddVertsForGrid2D(noiseVerts.GetMutableVerts(), perlinNoiseBounds, (Rgba8*) m_perlinNoise2D.GetRawData(), m_perlinNoise2D.GetDimensions(), Rgba8::Empty);

	// Render all text
	font->SetRendererState();
	g_renderer->DrawVertexBuffer(&textVerts);

	// Render all noise
	g_renderer->BindTexture(nullptr);
	g_renderer->BindShader(nullptr);
	g_renderer->DrawVertexBuffer(&noiseVerts);
}



//----------------------------------------------------------------------------------------------------------------------
void Noise2DTest::Shutdown()
{

}



//----------------------------------------------------------------------------------------------------------------------
void Noise2DTest::GenerateNoise(int seed)
{
	m_rawNoise2D.Initialize(DIMS, Rgba8::White);
	m_fractalNoise2D.Initialize(DIMS, Rgba8::White);
	m_perlinNoise2D.Initialize(DIMS, Rgba8::White);

	for (int y = 0; y < DIMS.y; ++y)
	{
		for (int x = 0; x < DIMS.x; ++x)
		{
			float fx = (float) x;
			float fy = (float) y;

			float noise = GetNoiseZeroToOne2D(x, y, seed);
			Rgba8 color = GetColorForNoise(noise);
			m_rawNoise2D.Set(x, y, color);

			float fractalNoise = GetFractalNoise2D(fx, fy, SCALE, 3, 0.5f, 2.f, true, seed);
			color = GetColorForNoise(fractalNoise);
			m_fractalNoise2D.Set(x, y, color);

			float perlinNoise = GetPerlinNoise2D(fx, fy, SCALE, 8, 0.5f, 2.f, true, seed);
			color = GetColorForNoise(perlinNoise);
			m_perlinNoise2D.Set(x, y, color);
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 Noise2DTest::GetColorForNoise(float noiseValue)
{
	float zeroToOne = (noiseValue / 2.f) + 0.5f;
	return Rgba8::Lerp(Rgba8::White, Rgba8::Black, zeroToOne);
}
