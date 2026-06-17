// Bradley Christensen - 2022-2026
#pragma once
#include "EntityID.h"
#include "AdminSystem.h"
#include "Engine/Core/ErrorUtils.h"
#include <typeindex>



class AdminSystem;
class System;



//----------------------------------------------------------------------------------------------------------------------
struct SystemContext
{
public:

    //----------------------------------------------------------------------------------------------------------------------
    // GET COMPONENTS
    //
    template <typename CType>
    CType* GetComponent(EntityID entityID) const;

    template <typename CType>
    CType const* GetComponentConst(EntityID entityID) const;

    template <typename CType>
    CType* GetComponent(GroupIter const& it) const;

    template <typename CType>
    CType const* GetComponentConst(GroupIter const& it) const;

    template <typename CType>
    CType& GetSingleton() const;

    template <typename CType>
    CType const& GetSingletonConst() const;

    //----------------------------------------------------------------------------------------------------------------------
    // GET COMPONENT STORAGE (To pull RTTI out of iteration loops for fastest performance)
    //
    template <typename CType>
    ArrayStorage<CType>& GetArrayStorage() const;

    template <typename CType>
    ArrayStorage<CType> const& GetArrayStorageConst() const;

    template <typename CType>
    MapStorage<CType>& GetMapStorage() const;

    template <typename CType>
    MapStorage<CType> const& GetMapStorageConst() const;

    template <typename CType>
    TagStorage<CType>& GetTagStorage() const;

    template <typename CType>
    TagStorage<CType> const& GetTagStorageConst() const;

    //----------------------------------------------------------------------------------------------------------------------
    // COMPONENT ITERATION
    //
    template <typename...CTypes>
    GroupIter Iterate() const;

    template <typename...CTypes>
	BitMask GetComponentBitMask() const;

    //----------------------------------------------------------------------------------------------------------------------
    // CREATE/DESTROY ENTITIES
    //
    EntityID CreateEntity(int searchBeginEntityID = 0) const;
    bool DestroyEntity(EntityID entityID) const;

    //----------------------------------------------------------------------------------------------------------------------
    // ADD COMPONENTS
    //
    template <typename CType, typename...Args>
    CType* AddComponent(EntityID entityID, Args const& ...args) const;

    template <typename CType>
    CType* AddComponent(EntityID entityID, CType const& copy) const;

    //----------------------------------------------------------------------------------------------------------------------
    // REMOVE COMPONENTS
    //
    template <typename CType>
    void RemoveComponent(EntityID entityID) const;

    void RemoveComponent(EntityID entityID, BitMask componentBit) const;

	int NumEntities() const;

	bool IsValid(EntityID entityID) const;

	bool DoesEntityHaveComponentsUnsafe(int entityIndex, BitMask componentBitMask) const;

protected:

    friend class SystemScheduler;

    SystemContext() = default;
    explicit SystemContext(System* system, float deltaSeconds = 0.f);

    bool IsComponentAccessValid(std::type_index componentType, bool isWriteAccess) const;
    bool IsComponentAccessValid(BitMask componentBitMask, bool isWriteAccess) const;
    bool HasFullECSAccess() const;

public:
	
    System const*   m_system                    = nullptr;
    float		    m_deltaSeconds				= 0.f;

    bool            m_isPreOrPostRun            = false;

    // System Splitting Params
    bool		    m_didSystemSplit	        = false;
    int	            m_startEntityID				= 0;
    int	            m_endEntityID				= MAX_ENTITIES - 1;
    int			    m_systemSplittingJobID		= 0; // Index ranging from 0 to (SystemSplittingNumJobs - 1)
    int             m_systemSplittingNumJobs    = 0;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType* SystemContext::GetComponent(EntityID entityID) const
{
	ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), true), "SystemContext::GetComponent - Does not have write access.");
	return g_ecs->GetComponent<CType>(entityID);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType const* SystemContext::GetComponentConst(EntityID entityID) const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), false), "SystemContext::GetComponentConst - Does not have read access.");
    return g_ecs->GetComponent<CType>(entityID);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType* SystemContext::GetComponent(GroupIter const& it) const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), true), "SystemContext::GetComponent - Does not have write access.");
    return g_ecs->GetComponent<CType>(it);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType const* SystemContext::GetComponentConst(GroupIter const& it) const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), false), "SystemContext::GetComponentConst - Does not have read access.");
    return g_ecs->GetComponent<CType>(it);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType& SystemContext::GetSingleton() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), true), "SystemContext::GetSingleton - Does not have write access.");
    return g_ecs->GetSingleton<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType const& SystemContext::GetSingletonConst() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), false), "SystemContext::GetSingletonConst - Does not have read access.");
    return g_ecs->GetSingleton<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
ArrayStorage<CType>& SystemContext::GetArrayStorage() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), true), "SystemContext::GetArrayStorage - Does not have write access.");
    return g_ecs->GetArrayStorage<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
ArrayStorage<CType> const& SystemContext::GetArrayStorageConst() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), false), "SystemContext::GetArrayStorageConst - Does not have read access.");
    return g_ecs->GetArrayStorage<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
MapStorage<CType>& SystemContext::GetMapStorage() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), true), "SystemContext::GetMapStorage - Does not have write access.");
    return g_ecs->GetMapStorage<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
MapStorage<CType> const& SystemContext::GetMapStorageConst() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), false), "SystemContext::GetMapStorageConst - Does not have read access.");
    return g_ecs->GetMapStorage<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
TagStorage<CType>& SystemContext::GetTagStorage() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), true), "SystemContext::GetTagStorage - Does not have write access.");
    return g_ecs->GetTagStorage<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
TagStorage<CType> const& SystemContext::GetTagStorageConst() const
{
    ASSERT_OR_DIE(IsComponentAccessValid(typeid(CType), false), "SystemContext::GetTagStorageConst - Does not have read access.");
    return g_ecs->GetTagStorage<CType>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
GroupIter SystemContext::Iterate() const
{
    if (m_isPreOrPostRun)
    {
        // System splitting should only apply to Run, not pre or post run.
		return g_ecs->IterateAll<CTypes...>();
    }
    else
    {
        return g_ecs->Iterate<CTypes...>(*this);
    }
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
BitMask SystemContext::GetComponentBitMask() const
{
	return g_ecs->GetComponentBitMask<CTypes...>();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType, typename...Args>
CType* SystemContext::AddComponent(EntityID entityID, Args const& ...args) const
{
    ASSERT_OR_DIE(HasFullECSAccess(), "SystemContext::AddComponent - System does not have full ECS access, cannot add component.");
    return g_ecs->AddComponent<CType>(entityID, args...);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType* SystemContext::AddComponent(EntityID entityID, CType const& copy) const
{
    ASSERT_OR_DIE(HasFullECSAccess(), "SystemContext::AddComponent - System does not have full ECS access, cannot add component.");
    return g_ecs->AddComponent<CType>(entityID, copy);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void SystemContext::RemoveComponent(EntityID entityID) const
{
    ASSERT_OR_DIE(HasFullECSAccess(), "SystemContext::RemoveComponent - System does not have full ECS access, cannot remove component.");
    g_ecs->RemoveComponent<CType>(entityID);
}