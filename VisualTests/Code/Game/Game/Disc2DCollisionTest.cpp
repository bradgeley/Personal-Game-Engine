// Bradley Christensen - 2023
#include "Disc2DCollisionTest.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Debug/DevConsole.h"



constexpr float MIN_ELAS = 0.f;
constexpr float MAX_ELAS = 0.5f;
constexpr float MIN_RADIUS = 0.25f;
constexpr float MAX_RADIUS = 4.f;
Vec2 const GRAVITY = Vec2(0.f, -20.f);



//----------------------------------------------------------------------------------------------------------------------
void Disc2DCollisionTest::Startup()
{
	m_camera.SetOrthoBounds(Vec3(0.f, 0.f, 0.f), Vec3(100.f, 50.f, 1.f));

	g_rng = new RandomNumberGenerator();
}



//----------------------------------------------------------------------------------------------------------------------
void Disc2DCollisionTest::Update(float deltaSeconds)
{
	if (g_input->IsMouseButtonDown(0))
	{
		m_spawnPos = m_camera.ScreenToWorldOrtho(g_input->GetMouseClientRelativePosition());
	}
	if (g_input->IsMouseButtonDown(1))
	{
		m_targetPos = m_camera.ScreenToWorldOrtho(g_input->GetMouseClientRelativePosition());
	}
	if (g_input->WasKeyJustPressed('G'))
	{
		m_gravityOn = !m_gravityOn;
	}
	if (g_input->WasKeyJustPressed(KeyCode::Space))
	{
		Vec2 velocity = (m_targetPos - m_spawnPos);
		float elasticity = g_rng->GetRandomFloatInRange(MIN_ELAS, MAX_ELAS);
		float radius = g_rng->GetRandomFloatInRange(0.25f, 4.f);
		float mass = radius;
		m_discs.emplace_back(m_spawnPos, velocity, elasticity, radius, mass);
	}

	static float accumulatedTime = 0.f;
	static float timeStep = 0.005f;
	static float maxAccumulation = 1.f;
	static float wallElasticity = 0.9f;

	// Add delta seconds to accumulate time
	accumulatedTime += deltaSeconds;

	// Adjust time step if time is accumulating too much and getting out of hand
	float timeStepThisFrame = timeStep;
	if (accumulatedTime > 1.f)
	{
		accumulatedTime = 1.f;
		timeStepThisFrame = 1.f;
	}

	while (accumulatedTime >= timeStepThisFrame)
	{
		// Physics Loop
		for (auto& disc : m_discs)
		{
			if (m_gravityOn)
			{
				disc.vel += GRAVITY * timeStepThisFrame;
			}
			disc.pos += disc.vel * timeStepThisFrame;
		}

		// Disc to disc collision
		for (int d1 = 0; d1 < m_discs.size(); ++d1)
		{
			Disc2D& disc1 = m_discs[d1];
			for (int d2 = d1 + 1; d2 < m_discs.size(); ++d2)
			{
				Disc2D& disc2 = m_discs[d2];

				BounceDiscsOffEachOther2D(disc1.pos, disc1.radius, disc1.vel, disc1.mass, disc2.pos, disc2.radius, disc2.vel, disc2.mass, disc1.elasticity * disc2.elasticity);
			}
		}

		// World bounds collision
		for (auto& disc : m_discs)
		{
			float wallCollisionElasticity = wallElasticity * disc.elasticity;
			AABB2 ortho = m_camera.GetOrthoBounds2D();
			if (disc.pos.x + disc.radius > ortho.maxs.x)
			{
				// Right side
				disc.pos.x = ortho.maxs.x - disc.radius;
				disc.vel.x = -disc.vel.x * wallCollisionElasticity;
			}
			if (disc.pos.x - disc.radius < ortho.mins.x)
			{
				// Left side
				disc.pos.x = ortho.mins.x + disc.radius;
				disc.vel.x = -disc.vel.x * wallCollisionElasticity;
			}
			if (disc.pos.y + disc.radius > ortho.maxs.y)
			{
				// Top side
				disc.pos.y = ortho.maxs.y - disc.radius;
				disc.vel.y = -disc.vel.y * wallCollisionElasticity;
			}
			if (disc.pos.y - disc.radius < ortho.mins.y)
			{
				// Bottom side
				disc.pos.y = ortho.mins.y + disc.radius;
				disc.vel.y = -disc.vel.y * wallCollisionElasticity;
			}
		}

		accumulatedTime -= timeStep;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void Disc2DCollisionTest::Render() const
{
	g_renderer->BeginWindow(g_window);
	g_renderer->BeginCamera(m_camera);
	g_renderer->ClearScreen(Rgba8::DarkGray);

	// Set up text rendering
	auto font = g_renderer->GetDefaultFont();

	VertexBuffer textVerts;
	VertexBuffer verts;

	AddVertsForLine2D(verts.GetMutableVerts(), m_spawnPos, m_targetPos, 0.1f, Rgba8::Yellow);

	for (auto& disc : m_discs)
	{
		AddVertsForDisc2D(verts.GetMutableVerts(), disc.pos, disc.radius, 16, Rgba8::Lerp(Rgba8::Blue, Rgba8::White, disc.elasticity));
	}

	// Render all text
	font->SetRendererState();
	g_renderer->DrawVertexBuffer(&textVerts);

	// Render all noise
	g_renderer->BindTexture(nullptr);
	g_renderer->BindShader(nullptr);
	g_renderer->DrawVertexBuffer(&verts);
}



//----------------------------------------------------------------------------------------------------------------------
void Disc2DCollisionTest::Shutdown()
{
	delete g_rng;
}



//----------------------------------------------------------------------------------------------------------------------
void Disc2DCollisionTest::DisplayHelpMessage() const
{
	g_devConsole->AddLine("Change ball spawn/target location: LMB/RMB");
	g_devConsole->AddLine("Gravity Toggle: G");
}
