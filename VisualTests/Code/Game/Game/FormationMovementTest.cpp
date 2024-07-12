// Bradley Christensen - 2024
#include "FormationMovementTest.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Window.h"
#include "Engine/Debug/DevConsole.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/Math/Grid2D.h"




//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Startup()
{
	m_worldBounds = AABB2(0.f, 0.f, g_window->GetDimensions().x, g_window->GetDimensions().y);
	m_camera.SetOrthoBounds2D(m_worldBounds);

	GenerateRandomFormation();
	GenerateRandomFormation();
	GenerateRandomFormation();
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

	// unit movement
	for (auto& unit : m_units)
	{
		//unit->m_pos += Vec2::MakeFromUnitCircleDegrees(unit->m_orientation) * 10.f * deltaSeconds;
	}

	// unit v unit collision
	for (int i = 0; i < m_units.size(); ++i)
	{
		auto& unitA = m_units[i];
		for (int j = i + 1; j < m_units.size(); ++j)
		{
			auto& unitB = m_units[j];
			PushDiscsOutOfEachOther2D(unitA->m_pos, unitA->m_radius, unitB->m_pos, unitB->m_radius);
		}
	}

	// unit v world collision
	for (auto& unit : m_units)
	{
		if (unit->m_pos.x + unit->m_radius > m_worldBounds.maxs.x)
		{
			unit->m_pos.x = m_worldBounds.maxs.x - unit->m_radius;
		}
		if (unit->m_pos.x - unit->m_radius < m_worldBounds.mins.x)
		{
			unit->m_pos.x = m_worldBounds.mins.x + unit->m_radius;
		}
		if (unit->m_pos.y + unit->m_radius > m_worldBounds.maxs.y)
		{
			unit->m_pos.y = m_worldBounds.maxs.y - unit->m_radius;
		}
		if (unit->m_pos.y - unit->m_radius < m_worldBounds.mins.y)
		{
			unit->m_pos.y = m_worldBounds.mins.y + unit->m_radius;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Render() const
{
	g_renderer->BeginCameraAndWindow(&m_camera, g_window);
	g_renderer->ClearScreen(Rgba8::Gray);

	VertexBuffer m_verts;
	AddVertsForWireBox2D(m_verts.GetMutableVerts(), m_worldBounds, 5.f, Rgba8::White);

	for (auto& formation : m_formations)
	{
		for (auto unit : formation->m_units)
		{
			AddVertsForDisc2D(m_verts.GetMutableVerts(), unit->m_pos, unit->m_radius, 16);
			AddVertsForLine2D(m_verts.GetMutableVerts(), unit->m_pos, unit->m_pos + (Vec2::MakeFromUnitCircleDegrees(unit->m_orientation) * unit->m_radius), 1.f, Rgba8::Red);
		}
		AddVertsForDisc2D(m_verts.GetMutableVerts(), formation->m_pos, 10.f, 16, Rgba8::Green);
		AddVertsForLine2D(m_verts.GetMutableVerts(), formation->m_pos, formation->m_pos + (Vec2::MakeFromUnitCircleDegrees(formation->m_orientation) * 10.f), 1.f, Rgba8::Red);
	}

	Vec2 mouseRelativePos = g_input->GetMouseClientRelativePosition();
	Vec2 mouseWorldPos = m_camera.ScreenToWorldOrtho(mouseRelativePos);
	AddVertsForDisc2D(m_verts.GetMutableVerts(), mouseWorldPos, 5.f, 16, Rgba8::Blue);

	// Coordinate System Debug draw
	AddVertsForLine2D(m_verts.GetMutableVerts(), Vec2(0.f, 0.f), Vec2(0.f, 10.f), 1.f, Rgba8::Green);
	AddVertsForLine2D(m_verts.GetMutableVerts(), Vec2(0.f, 0.f), Vec2(10.f, 0.f), 1.f, Rgba8::Red);

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
	int numUnits = m_rng.GetRandomIntInRange(0, 20);
	float unitRadii = m_rng.GetRandomFloatInRange(10.f, 30.f);
	constexpr int maxUnitsPerRow = 5;

	int numRows = (numUnits + maxUnitsPerRow - 1) / maxUnitsPerRow;
	Grid2D<int> grid(maxUnitsPerRow, numRows, 0);

	formation->m_pos = m_rng.GetRandomVecInRange2D(m_worldBounds.mins + Vec2(100.f, 100.f), m_worldBounds.maxs - Vec2(100.f, 100.f));
	formation->m_orientation = 90.f;

	Vec2 formationHalfDims;
	formationHalfDims.x = (float) maxUnitsPerRow * unitRadii;
	formationHalfDims.y = (float) numRows * unitRadii;

	for (int i = 0; i < numUnits; ++i)
	{
		Unit* unit = new Unit();
		auto coords = grid.GetCoordsForIndex(i);
		auto formationOffset = Vec2(coords.x * unitRadii, -coords.y * unitRadii) * 2.f;
		formationOffset -= formationHalfDims;
		formation->m_localFormationOffsets.push_back(formationOffset);
		Vec2 rotatedOffset = formationOffset.GetRotated(formation->m_orientation);
		unit->m_pos = formation->m_pos + rotatedOffset;
		unit->m_radius = unitRadii;
		unit->m_orientation =formation->m_orientation;
		unit->m_formation = formation;
		formation->m_units.push_back(unit);
		m_units.push_back(unit);
	}
	m_formations.push_back(formation);
}
