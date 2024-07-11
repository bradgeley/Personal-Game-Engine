// Bradley Christensen - 2023
#pragma once
#include "VisualTest.h"
#include "Engine/Math/Vec2.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct Disc2D
{
	Disc2D(Vec2 const& pos, Vec2 const& vel, float elas, float rad, float mass) : pos(pos), vel(vel), elasticity(elas), radius(rad), mass(mass) {}

	Vec2 pos			= Vec2::ZeroVector;
	Vec2 vel			= Vec2::ZeroVector;
	float elasticity	= 0.9f;
	float radius		= 1.f;
	float mass			= 0.f;
};



//----------------------------------------------------------------------------------------------------------------------
class Disc2DCollisionTest : public VisualTest
{
public:

	Disc2DCollisionTest() { m_name = "Disc2D Collision Test"; }

	void Startup() override;
	void Update(float deltaSeconds) override;
	void Render() const override;
	void Shutdown() override;

	virtual void DisplayHelpMessage() const override;

	bool m_gravityOn = false;
	Vec2 m_spawnPos = Vec2::ZeroVector;
	Vec2 m_targetPos = Vec2::ZeroVector;
	std::vector<Disc2D> m_discs;
};

