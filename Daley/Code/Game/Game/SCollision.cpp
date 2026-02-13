// Bradley Christensen - 2022-2025
#include "SCollision.h"
#include "CCollision.h"
#include "SCCollision.h"
#include "SCWorld.h"
#include "CTransform.h"
#include "Engine/Math/GeometryUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Startup()
{
    AddWriteDependencies<CTransform>();
    AddReadDependencies<SCCollision, SCWorld, CCollision>();
}



//----------------------------------------------------------------------------------------------------------------------
void SCollision::Run(SystemContext const&)
{
    //SCCollision& scCollision = g_ecs->GetSingleton<SCCollision>();
	//
	//auto& transformStorage = g_ecs->GetArrayStorage<CTransform>();
	//auto& collisionStorage = g_ecs->GetArrayStorage<CCollision>();
	//
	//SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();

}