// Bradley Christensen - 2024
#pragma once
#include "VisualTest.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Camera.h"
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
	std::vector<Unit*> m_units;
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

	Camera m_camera;
	AABB2 m_worldBounds;
	std::vector<Formation*> m_formations;
};