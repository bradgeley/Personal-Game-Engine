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
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/StatisticsUtils.h"
#include "Engine/Math/Grid2D.h"
#include "Engine/DataStructures/BitArray.h"
#include <queue>



constexpr float UNIT_TURN_RATE = 45.f;
constexpr float UNIT_MOVE_SPEED = 10.f;
constexpr float TILE_SIZE = 10.f;
const IntVec2 GRID_DIMS = IntVec2(100, 100);
constexpr float CAMERA_HEIGHT = TILE_SIZE * 100;
constexpr float CAMERA_PAN_SPEED = 100.f;


//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Startup()
{
	m_worldBounds = AABB2(0.f, 0.f, GRID_DIMS.x * TILE_SIZE, GRID_DIMS.y * TILE_SIZE);
	m_camera.SetOrthoDims2D(Vec2(CAMERA_HEIGHT * g_window->GetAspect(), CAMERA_HEIGHT));
	m_worldTiles.Initialize(GRID_DIMS, 0);
	m_worldTileBounds.Initialize(GRID_DIMS, AABB2());
	m_worldVerts = new VertexBuffer();
	m_worldVerts->GetMutableVerts().reserve(6 * m_worldTiles.Size());

	AddVertsForAABB2(m_worldVerts->GetMutableVerts(), m_worldBounds, Rgba8::LightGray);

	for (int i = 0; i < m_worldTiles.Size(); ++i)
	{
		IntVec2 coords = m_worldTiles.GetCoordsForIndex(i);
		IntVec2 topRightCoords = coords + IntVec2(1, 1);
		AABB2 tileBounds = AABB2(Vec2(coords) * TILE_SIZE, Vec2(topRightCoords) * TILE_SIZE);
		m_worldTileBounds.Set(i, tileBounds);
		if (m_rng.GetRandomFloatZeroToOne() < 0.1f)
		{
			m_worldTiles.Set(i, true);
			AddVertsForAABB2(m_worldVerts->GetMutableVerts(), tileBounds, Rgba8::DarkGray);
		}
	}

	AddVertsForWireGrid2D(m_worldVerts->GetMutableVerts(), m_worldBounds, GRID_DIMS, 0.5f, Rgba8::Black);

	GenerateRandomFormation();
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Update(float deltaSeconds)
{
	// Camera Movement
	int mouseWheelMovement = g_input->GetMouseWheelChange();
	if (mouseWheelMovement != 0)
	{
		Vec2 mouseRelativePos = g_input->GetMouseClientRelativePosition();
		Vec2 mouseWorldPos = m_camera.ScreenToWorldOrtho(mouseRelativePos);
		float zoomRatio = (1.f - 0.05f * (float) mouseWheelMovement);
		m_camera.ZoomAroundCenter2D(zoomRatio, mouseWorldPos);
	}
	if (g_input->IsKeyDown('W')) m_camera.Translate2D(CAMERA_PAN_SPEED * Vec2(0.f, 1.f));
	if (g_input->IsKeyDown('A')) m_camera.Translate2D(CAMERA_PAN_SPEED * Vec2(-1.f, 0.f));
	if (g_input->IsKeyDown('S')) m_camera.Translate2D(CAMERA_PAN_SPEED * Vec2(0.f, -1.f));
	if (g_input->IsKeyDown('D')) m_camera.Translate2D(CAMERA_PAN_SPEED * Vec2(1.f, 0.f));

	// Calculate formation path
	for (auto& formation : m_formations)
	{
		IntVec2 currentTile = IntVec2(formation->m_pos.x / TILE_SIZE, formation->m_pos.y / TILE_SIZE);
		bool succeeded = GeneratePath(currentTile, formation->m_destinationTile, formation->m_currentPath);
		if (!succeeded)
		{
			formation->m_destinationTile = IntVec2(m_rng.GetRandomVecInRange2D(m_worldBounds.mins, m_worldBounds.maxs) / TILE_SIZE);
		}
	}

	// unit movement
	for (auto& unit : m_units)
	{
		Vec2 velocity = Vec2::MakeFromUnitCircleDegrees(unit->m_orientation) * UNIT_MOVE_SPEED;
		//unit->m_pos += velocity * deltaSeconds;
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

	// unit v tile collision
	for (auto& unit : m_units)
	{
		IntVec2 tileCoords = IntVec2(unit->m_pos / TILE_SIZE);
		int tileIndex = m_worldTiles.GetIndexForCoords(tileCoords);
		int neighbors[4] =
		{
			m_worldTiles.GetIndexNorthOf(tileIndex),
			m_worldTiles.GetIndexSouthOf(tileIndex),
			m_worldTiles.GetIndexEastOf(tileIndex),
			m_worldTiles.GetIndexWestOf(tileIndex)
		};

		for (auto& neighbor : neighbors)
		{
			if (neighbor == -1)
			{
				continue;
			}
			AABB2 tileBounds = m_worldTileBounds.Get(neighbor);
			PushDiscOutOfAABB2D(unit->m_pos, unit->m_radius, tileBounds);
		}
	}

	// unit v world bounds collision
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

	// Recalculate formation position
	for (auto& formation : m_formations)
	{
		Vec2 totalPosition = Vec2::ZeroVector; 
		for (auto& unit : formation->m_units)
		{
			totalPosition += unit->m_pos;
		}
		formation->m_pos = totalPosition / (float) formation->m_units.size();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void FormationMovementTest::Render() const
{
	g_renderer->BeginCameraAndWindow(&m_camera, g_window);
	g_renderer->ClearScreen(Rgba8::White);

	g_renderer->DrawVertexBuffer(m_worldVerts);

	VertexBuffer verts;
	verts.GetMutableVerts().reserve(10000);

	for (auto& formation : m_formations)
	{
		for (auto& pathPt : formation->m_currentPath)
		{
			AABB2 tile = AABB2(pathPt.x * TILE_SIZE, pathPt.y * TILE_SIZE, (pathPt.x + 1) * TILE_SIZE, (pathPt.y + 1) * TILE_SIZE);
			AddVertsForAABB2(verts.GetMutableVerts(), tile);
		}

		for (int i = 0; i < formation->m_units.size(); ++i)
		{
			auto unit = formation->m_units[i];
			AddVertsForDisc2D(verts.GetMutableVerts(), unit->m_pos, unit->m_radius, 16);
			AddVertsForLine2D(verts.GetMutableVerts(), unit->m_pos, unit->m_pos + (Vec2::MakeFromUnitCircleDegrees(unit->m_orientation) * unit->m_radius), 1.f, Rgba8::Red);
			AddVertsForDisc2D(verts.GetMutableVerts(), formation->m_pos + formation->m_localFormationOffsets[i].GetRotated(formation->m_orientation), 2.5f, 16, Rgba8::Blue);
		}
		AddVertsForDisc2D(verts.GetMutableVerts(), formation->m_pos, 10.f, 16, Rgba8::Green);
		AddVertsForLine2D(verts.GetMutableVerts(), formation->m_pos, formation->m_pos + (Vec2::MakeFromUnitCircleDegrees(formation->m_orientation) * 10.f), 1.f, Rgba8::Red);
	}

	Vec2 mouseRelativePos = g_input->GetMouseClientRelativePosition();
	Vec2 mouseWorldPos = m_camera.ScreenToWorldOrtho(mouseRelativePos);
	AddVertsForDisc2D(verts.GetMutableVerts(), mouseWorldPos, 5.f, 16, Rgba8::Blue);

	// Coordinate System Debug draw
	AddVertsForLine2D(verts.GetMutableVerts(), Vec2(0.f, 0.f), Vec2(0.f, 10.f), 1.f, Rgba8::Green);
	AddVertsForLine2D(verts.GetMutableVerts(), Vec2(0.f, 0.f), Vec2(10.f, 0.f), 1.f, Rgba8::Red);

	g_renderer->DrawVertexBuffer(&verts);
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

	delete m_worldVerts;
	m_worldVerts = nullptr;
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
	int numUnits = 10;
	float unitRadii = TILE_SIZE * 0.5f;
	float unitDiameter = unitRadii * 2.f;
	constexpr int maxUnitsPerRow = 5;

	int numRows = (numUnits + maxUnitsPerRow - 1) / maxUnitsPerRow;
	Grid2D<int> grid(maxUnitsPerRow, numRows, 0);

	formation->m_pos = m_rng.GetRandomVecInRange2D(m_worldBounds.mins + Vec2(100.f, 100.f), m_worldBounds.maxs - Vec2(100.f, 100.f));
	formation->m_orientation = m_rng.GetRandomFloatInRange(10.f, 360.f);

	Vec2 formationHalfDims;
	formationHalfDims.x = (float) maxUnitsPerRow * unitRadii;
	formationHalfDims.y = (float) numRows * unitRadii;

	formation->m_destinationTile.x = g_rng->GetRandomIntInRange(0, m_worldTiles.GetDimensions().x - 1);
	formation->m_destinationTile.y = g_rng->GetRandomIntInRange(0, m_worldTiles.GetDimensions().y - 1);

	for (int i = 0; i < numUnits; ++i)
	{
		Unit* unit = new Unit();
		auto coords = grid.GetCoordsForIndex(i);
		auto formationOffset = Vec2(-coords.y * unitDiameter, coords.x * unitDiameter);
		formationOffset.y -= formationHalfDims.x - unitRadii;
		formationOffset.x += formationHalfDims.y - unitRadii;
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



//----------------------------------------------------------------------------------------------------------------------
struct DijkstraTile
{
	int m_index = -1;
	float m_distance = FLT_MAX;
};



//----------------------------------------------------------------------------------------------------------------------
class DijkstraTileDistanceCompare
{
public:
	bool operator()(DijkstraTile const& a, DijkstraTile const& b)
	{
		return (a.m_distance > b.m_distance);
	}
};



//----------------------------------------------------------------------------------------------------------------------
bool FormationMovementTest::GeneratePath(IntVec2 const& start, IntVec2 const& end, std::vector<IntVec2>& out_pathTiles) const
{
	Grid2D<bool> visitedNodes(m_worldTiles.GetDimensions(), false);
	Grid2D<float> tileDistance(m_worldTiles.GetDimensions(), FLT_MAX);
	tileDistance.Set(start, 0);

	std::priority_queue<DijkstraTile, std::vector<DijkstraTile>, DijkstraTileDistanceCompare> unvisitedSet;
	for (int i = 0; i < m_worldTiles.Size(); ++i)
	{
		DijkstraTile tile = DijkstraTile{ i, tileDistance.Get(i) };
		unvisitedSet.push(tile);
	}

	while (true)
	{
		DijkstraTile currentTile = unvisitedSet.top();
		unvisitedSet.pop();

		if (currentTile.m_index == -1)
		{
			break;
		}

		float visitingNodeDistance = tileDistance.Get(currentTile.m_index);

		int neighbors[4] = 
		{ 
			m_worldTiles.GetIndexNorthOf(currentTile.m_index),
			m_worldTiles.GetIndexSouthOf(currentTile.m_index),
			m_worldTiles.GetIndexEastOf(currentTile.m_index),
			m_worldTiles.GetIndexWestOf(currentTile.m_index)
		};

		for (auto& neighborIndex : neighbors)
		{
			if (neighborIndex == -1)
			{
				continue;
			}

			if (visitedNodes.Get(neighborIndex))
			{
				// Neighbor has already been visited
				continue;
			}

			if (m_worldTiles.Get(neighborIndex) != 0)
			{
				// Neighbor tile is blocked
				continue;
			}

			if (!visitedNodes.Get(neighborIndex))
			{
				float neighborCurrentDistance = tileDistance.Get(neighborIndex);
				tileDistance.Set(neighborIndex, MinF(neighborCurrentDistance, visitingNodeDistance + 1));
			}
		}

		if (m_worldTiles.GetCoordsForIndex(currentTile.m_index) == end)
		{
			break;
		}
	}

	std::vector<IntVec2> backstep;
	backstep.reserve(100);
	backstep.push_back(end);
	while (true)
	{
		IntVec2& currentCoords = backstep[backstep.size() - 1];
		int currentIndex = m_worldTiles.GetIndexForCoords(currentCoords);
		int neighbors[4] =
		{
			m_worldTiles.GetIndexNorthOf(currentIndex),
			m_worldTiles.GetIndexSouthOf(currentIndex),
			m_worldTiles.GetIndexEastOf(currentIndex),
			m_worldTiles.GetIndexWestOf(currentIndex)
		};

		float currentDistance = tileDistance.Get(currentIndex);

		int bestNeighbor = -1;
		float bestNeighborDistance = FLT_MAX;
		for (auto& neighborIndex : neighbors)
		{
			if (neighborIndex == -1)
			{
				continue;
			}
			float neighborDistance = tileDistance.Get(neighborIndex);
			if (neighborDistance < bestNeighborDistance)
			{
				bestNeighborDistance = neighborDistance;
				bestNeighbor = neighborIndex;
			}
		}

		if (bestNeighbor && bestNeighborDistance < currentDistance)
		{
			backstep.push_back(m_worldTiles.GetCoordsForIndex(bestNeighbor));
		}
		else break;
	}

	bool containsStart = false;
	bool containsEnd = false;
	for (int i = backstep.size() - 1; i >= 0; --i)
	{
		out_pathTiles.push_back(backstep[i]);

		if (backstep[i] == start)
		{
			containsStart = true;
		}

		if (backstep[i] == end)
		{
			containsEnd = true;
		}
	}
	return containsStart && containsEnd;

	
}
