// Bradley Christensen - 2023
#include "SPhysics.h"
#include "Game/UnitTests/ECS/Components/CPhysics.h"
#include "Game/UnitTests/ECS/Components/CTransform2D.h"
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Startup()
{
	AddWriteDependencies<CPhysics, CTransform2D>();

	m_systemSplittingNumJobs = (int) std::thread::hardware_concurrency() - 1;
}



//----------------------------------------------------------------------------------------------------------------------
void SPhysics::Run(SystemContext const& context)
{
	auto& physicsStorage = m_admin->GetArrayStorage<CPhysics>();
	auto& transformStorage = m_admin->GetArrayStorage<CTransform2D>();

	for (auto it = m_admin->GroupIterBegin<CPhysics, CTransform2D>(context); it.IsValid(); it.Next())
	{
		auto ent = it.m_currentIndex;
		CPhysics& p = physicsStorage[ent];
		CTransform2D& t = transformStorage[ent];
		p.m_velocity += (p.m_acceleration) * context.m_deltaSeconds;
		t.m_screenPosition += p.m_velocity * context.m_deltaSeconds;

		p.m_acceleration = Vec2::ZeroVector;
	}
}
