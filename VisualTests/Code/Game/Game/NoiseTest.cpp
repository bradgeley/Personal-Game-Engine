// Bradley Christensen - 2023
#include "NoiseTest.h"
#include "Engine/Math/Noise.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Font.h"



//----------------------------------------------------------------------------------------------------------------------
void NoiseTest::Startup()
{
	m_camera.SetOrthoBounds(Vec3(0.f, 0.f, 0.f), Vec3(100.f, 50.f, 1.f));

	GenerateNoise(m_seed);


}



//----------------------------------------------------------------------------------------------------------------------
void NoiseTest::Update(float deltaSeconds)
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
void NoiseTest::Render() const
{
	g_renderer->BeginWindow(g_window);
	g_renderer->BeginCamera(m_camera);
	g_renderer->ClearScreen(Rgba8::DarkGray);

	// Set up text rendering
	auto font = g_renderer->GetDefaultFont();

	VertexBuffer textVerts;
	VertexBuffer rawNoise;

	// Raw noise
	Vec2 offset = Vec2(15.f, 2.f);
	font->AddVertsForAlignedText2D(textVerts.GetMutableVerts(), Vec2(0.f, offset.y), Vec2(1.f, 0.f), 3.f, "Raw Noise", Rgba8::White);
	AddVertsForLine2D(rawNoise.GetMutableVerts(), offset + Vec2(0.f, 1.f), offset + Vec2(200.f, 1.f), 0.1f, Rgba8::LightGray);
	AddVertsForLine2D(rawNoise.GetMutableVerts(), offset + Vec2(0.f, -1.f), offset + Vec2(200.f, -1.f), 0.1f, Rgba8::LightGray);
	for (int i = 0; i < m_rawNoise1D.size() - 1; ++i)
	{
		Vec2 start = offset + Vec2(float(i) * 0.1f, m_rawNoise1D[i]);
		Vec2 end = offset + Vec2(float(i + 1) * 0.1f, m_rawNoise1D[i + 1]);
		AddVertsForLine2D(rawNoise.GetMutableVerts(), start, end, 0.1f, Rgba8::LightBlue);
	}

	// Fractal noise
	offset = Vec2(15.f, 5.f);
	font->AddVertsForAlignedText2D(textVerts.GetMutableVerts(), Vec2(0.f, offset.y), Vec2(1.f, 0.f), 3.f, "Fractal Noise", Rgba8::White);
	AddVertsForLine2D(rawNoise.GetMutableVerts(), offset + Vec2(0.f, 1.f), offset + Vec2(200.f, 1.f), 0.1f, Rgba8::LightGray);
	AddVertsForLine2D(rawNoise.GetMutableVerts(), offset + Vec2(0.f, -1.f), offset + Vec2(200.f, -1.f), 0.1f, Rgba8::LightGray);
	for (int i = 0; i < m_fractalNoise1D.size() - 1; ++i)
	{
		Vec2 start = offset + Vec2(float(i) * 0.1f, m_fractalNoise1D[i]);
		Vec2 end = offset + Vec2(float(i + 1) * 0.1f, m_fractalNoise1D[i + 1]);
		AddVertsForLine2D(rawNoise.GetMutableVerts(), start, end, 0.1f, Rgba8::LightBlue);
	}

	// Perlin noise
	offset = Vec2(15.f, 8.f);
	font->AddVertsForAlignedText2D(textVerts.GetMutableVerts(), Vec2(0.f, offset.y), Vec2(1.f, 0.f), 3.f, "Perlin Noise", Rgba8::White);
	AddVertsForLine2D(rawNoise.GetMutableVerts(), offset + Vec2(0.f, 1.f), offset + Vec2(200.f, 1.f), 0.1f, Rgba8::LightGray);
	AddVertsForLine2D(rawNoise.GetMutableVerts(), offset + Vec2(0.f, -1.f), offset + Vec2(200.f, -1.f), 0.1f, Rgba8::LightGray);
	for (int i = 0; i < m_perlinNoise1D.size() - 1; ++i)
	{
		Vec2 start = offset + Vec2(float(i) * 0.1f, m_perlinNoise1D[i]);
		Vec2 end = offset + Vec2(float(i + 1) * 0.1f, m_perlinNoise1D[i + 1]);
		AddVertsForLine2D(rawNoise.GetMutableVerts(), start, end, 0.1f, Rgba8::LightBlue);
	}

	// Render all text
	font->SetRendererState();
	g_renderer->DrawVertexBuffer(&textVerts);

	// Render all noise
	g_renderer->BindTexture(nullptr);
	g_renderer->BindShader(nullptr);
	g_renderer->DrawVertexBuffer(&rawNoise);
}



//----------------------------------------------------------------------------------------------------------------------
void NoiseTest::Shutdown()
{

}



//----------------------------------------------------------------------------------------------------------------------
void NoiseTest::GenerateNoise(int seed)
{
	m_rawNoise1D.clear();
	m_fractalNoise1D.clear();
	m_perlinNoise1D.clear();
	for (float i = 0.f; i < 100.f; i += 0.25f)
	{
		m_rawNoise1D.emplace_back(GetNoiseZeroToOne1D(i, seed));
		m_fractalNoise1D.emplace_back(GetFractalNoise1D(i, 5.f, 5, 0.5f, 2.f, true, seed));
		m_perlinNoise1D.emplace_back(GetPerlinNoise1D(i, 5.f, 5, 0.5f, 2.f, true, seed));
	}
}
