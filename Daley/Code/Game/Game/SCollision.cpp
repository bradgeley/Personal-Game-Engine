// Bradley Christensen - 2022-2026
#include "SCollision.h"
#include "CCollision.h"
#include "SCCollision.h"
#include "CTransform.h"
#include "Engine/Math/GeometryUtils.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddReadDependencies<CCollision, CTransform>();
    AddWriteDependencies<SCCollision>();

	m_runWhilePaused = false;
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const& context) const
{
	// Read Dependencies
	auto& transformStorage = context.GetArrayStorageConst<CTransform>();
	auto& collisionStorage = context.GetArrayStorageConst<CCollision>();

	// Write Dependencies
    SCCollision& scCollision = context.GetSingleton<SCCollision>();

}