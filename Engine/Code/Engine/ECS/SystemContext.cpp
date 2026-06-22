// Bradley Christensen - 2022-2026
#include "SystemContext.h"
#include "AdminSystem.h"
#include "System.h"



//----------------------------------------------------------------------------------------------------------------------
SystemContext::SystemContext(System* system, float deltaSeconds /*= 0.f*/) : m_system(system), m_deltaSeconds(deltaSeconds)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool SystemContext::IsComponentAccessValid(std::type_index componentType, bool isWriteAccess) const
{
	BitMask componentBit = g_ecs->GetComponentBit(componentType);
	return IsComponentAccessValid(componentBit, isWriteAccess);
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemContext::IsComponentAccessValid(BitMask componentBitMask, bool isWriteAccess) const
{
	if (isWriteAccess)
	{
		return (m_system->GetWriteDependencies() & componentBitMask) != 0;
	}
	else
	{
		bool isWriteOk = (m_system->GetWriteDependencies() & componentBitMask) != 0;
		bool isReadOk = (m_system->GetReadDependencies() & componentBitMask) != 0;
		return isWriteOk || isReadOk;
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemContext::HasFullECSAccess() const
{
    return m_system->GetWriteDependencies() == SIZE_MAX;
}



//----------------------------------------------------------------------------------------------------------------------
EntityID SystemContext::CreateEntity(int searchBeginEntityID /*= 0*/) const
{
	ASSERT_OR_DIE(HasFullECSAccess(), "SystemContext::CreateEntity - System does not have full ECS access, cannot create entity.");
	return g_ecs->CreateEntity(searchBeginEntityID);
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemContext::DestroyEntity(EntityID entityID) const
{
    ASSERT_OR_DIE(HasFullECSAccess(), "SystemContext::DestroyEntity - System does not have full ECS access, cannot destroy entity.");
    return g_ecs->DestroyEntity(entityID);
}



//----------------------------------------------------------------------------------------------------------------------
void SystemContext::RemoveComponent(EntityID entityID, BitMask componentBit) const
{
    ASSERT_OR_DIE(HasFullECSAccess(), "SystemContext::RemoveComponent - System does not have full ECS access, cannot remove component.");
    g_ecs->RemoveComponent(entityID, componentBit);
}



//----------------------------------------------------------------------------------------------------------------------
int SystemContext::NumEntities() const
{
	return g_ecs->NumEntities();
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemContext::IsValid(EntityID entityID) const
{
	return g_ecs->IsValid(entityID);
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemContext::HasComponents(EntityID entityID, BitMask componentBitMask) const
{
	return g_ecs->HasComponents(entityID, componentBitMask);
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemContext::HasComponentsUnsafe(int entityIndex, BitMask componentBitMask) const
{
	return g_ecs->HasComponentsUnsafe(entityIndex, componentBitMask);
}