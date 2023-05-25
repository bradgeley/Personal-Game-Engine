#include "SMovement.h"
#include "Game/UnitTests/ECS/Components/CMovement.h"
#include "Game/UnitTests/ECS/Components/CPhysics.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Startup()
{
	AddWriteDependencies<CPhysics, CMovement>();

	m_systemSplittingNumJobs = (int) std::thread::hardware_concurrency() - 1;
}



//----------------------------------------------------------------------------------------------------------------------
void UpdateEntity(CPhysics& physics, CMovement& movement)
{
	if (movement.m_frameMoveDirection.GetLengthSquared() > 0.f)
	{
		movement.m_frameMoveDirection.Normalize();
		if (movement.m_frameMoveStrength > 1.f)
			movement.m_frameMoveStrength = 1.f;
		if (movement.m_frameMoveStrength < -1.f)
			movement.m_frameMoveStrength = -1.f;
	}

	if ((movement.m_movementFlags & (uint8_t) MovementFlag::Acceleration) != 0)
	{
		physics.m_acceleration += movement.m_frameMoveDirection * movement.m_frameMoveStrength * movement.m_movementSpeed;
		Vec2 drag = physics.m_velocity * movement.m_movementSpeed;
		drag.ClampLength(physics.m_velocity.GetLength());
		physics.m_acceleration -= drag;
	}
	else if ((movement.m_movementFlags & (uint8_t) MovementFlag::Velocity) != 0)
	{
		physics.m_velocity = movement.m_frameMoveDirection * movement.m_frameMoveStrength * movement.m_movementSpeed;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SMovement::Run(SystemContext const& context)
{
	auto& physicsStorage = m_admin->GetArrayStorage<CPhysics>();
	auto& movementStorage = m_admin->GetArrayStorage<CMovement>();

	for (auto it = m_admin->GroupIterBegin<CPhysics, CMovement>(context); it.IsValid(); it.Next())
	{
		auto& ent = it.m_currentIndex;

		CPhysics& physics = physicsStorage[ent];
		CMovement& movement = movementStorage[ent];
 
		UpdateEntity(physics, movement);
	}
}
