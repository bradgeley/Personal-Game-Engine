// Bradley Christensen - 2024
#include "FormationMovementTest.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Input/InputSystem.h"




//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Startup()
{
	m_worldBounds = AABB2(0.f, 0.f, g_window->GetDimensions().x, g_window->GetDimensions().y);
	m_camera.SetOrthoBounds2D(m_worldBounds);

	GenerateRandomFormation();
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Update(float deltaSeconds)
{
	int mouseWheelMovement = g_input->GetMouseWheelChange();
	if (mouseWheelMovement != 0)
	{
		Vec2 mouseRelativePos = g_input->GetMouseClientRelativePosition();
		Vec2 mouseWorldPos = m_camera.ScreenToWorldOrtho(mouseRelativePos);
		float zoomRatio = (1 - 0.05 * mouseWheelMovement);
		m_camera.ZoomAroundCenter2D(zoomRatio, mouseWorldPos);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Render() const
{
	g_renderer->BeginCameraAndWindow(&m_camera, g_window);
	g_renderer->ClearScreen(Rgba8::Gray);

	VertexBuffer m_verts;
	for (auto& formation : m_formations)
	{
		for (auto unit : formation->m_units)
		{
			AddVertsForDisc2D(m_verts.GetMutableVerts(), unit->m_pos, unit->m_radius, 16);
			AddVertsForLine2D(m_verts.GetMutableVerts(), unit->m_pos, unit->m_pos + (Vec2::MakeFromUnitCircleDegrees(unit->m_orientation) * 20.f), 1.f, Rgba8::Red);
		}
	}

	Vec2 mouseRelativePos = g_input->GetMouseClientRelativePosition();
	Vec2 mouseWorldPos = m_camera.ScreenToWorldOrtho(mouseRelativePos);
	AddVertsForDisc2D(m_verts.GetMutableVerts(), mouseWorldPos, 5.f, 16, Rgba8::Blue);

	g_renderer->DrawVertexBuffer(&m_verts);
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Shutdown()
{
	for (auto& formation : m_formations)
	{
		for (auto unit : formation->m_units)
		{
			delete unit;
			unit = nullptr;
		}
		formation->m_units.clear();
		delete formation;
		formation = nullptr;
	}
	m_formations.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::DisplayHelpMessage() const
{
	g_devConsole->AddLine("This is the formation movement Test");
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::GenerateRandomFormation()
{
	Formation* formation = new Formation();
	int numUnits = m_rng.GetRandomIntInRange(0, 10);
	float unitRadii = m_rng.GetRandomFloatInRange(10.f, 30.f);
	for (int i = 0; i < numUnits; ++i)
	{
		Unit* unit = new Unit();
		unit->m_pos = m_rng.GetRandomVecInRange2D(m_worldBounds.mins, m_worldBounds.maxs);
		unit->m_radius = unitRadii;
		unit->m_orientation = m_rng.GetRandomFloatInRange(0.f, 360.f);
		unit->m_formation = formation;
		formation->m_units.push_back(unit);
	}
	m_formations.resize(1);
	m_formations[0] = formation;
}
