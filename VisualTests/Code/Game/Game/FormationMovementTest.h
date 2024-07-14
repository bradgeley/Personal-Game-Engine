// Bradley Christensen - 2024
#pragma once
#include "VisualTest.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/Grid2D.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/VertexBuffer.h"
#include <vector>



struct Formation;



//----------------------------------------------------------------------------------------------------------------------
struct Unit
{
	Vec2 m_pos = Vec2::ZeroVector;
	float m_orientation = 0.f;
	float m_radius = 0.f;
	Formation* m_formation = nullptr;
};



struct Formation
{
	Vec2 m_pos = Vec2::ZeroVector;
	float m_orientation = 0.f;
	std::vector<Unit*> m_units;
	std::vector<Vec2> m_localFormationOffsets;
	std::vector<IntVec2> m_currentPath;
	IntVec2 m_destinationTile;
	Grid2D<int> m_distanceField;
};



//----------------------------------------------------------------------------------------------------------------------
class FormationMovementTest : public VisualTest
{
public:

	FormationMovementTest() { m_name = "Formation Movement Test"; }

	void Startup() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	virtual void DisplayHelpMessage() const override;

	void GenerateRandomFormation();
	bool GeneratePath(IntVec2 const& start, IntVec2 const& end, std::vector<IntVec2>& out_pathTiles) const;

	Camera m_camera;
	AABB2 m_worldBounds;
	Grid2D<uint8_t> m_worldTiles;
	Grid2D<AABB2> m_worldTileBounds;
	std::vector<Formation*> m_formations;
	std::vector<Unit*> m_units;

	VertexBuffer* m_worldVerts;
};