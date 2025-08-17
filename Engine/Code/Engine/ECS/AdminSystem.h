// Bradley Christensen - 2023
#pragma once
#include "ComponentStorage.h"
#include "GroupIter.h"
#include "EntityID.h"
#include "SystemSubgraph.h"
#include "SystemContext.h"
#include <vector>
#include <unordered_map>



class System;
class TaskSystem;
class SystemScheduler;



//----------------------------------------------------------------------------------------------------------------------
// THE ecs
//
extern class AdminSystem* g_ecs;



//----------------------------------------------------------------------------------------------------------------------
struct AdminSystemConfig
{
	float	m_maxDeltaSeconds						= -1.f; // Anything <= 0 means no cap
	bool	m_enableMultithreading					= false;
	int		m_autoMultithreadingEntityThreshold		= 0;
	int		m_systemSplittingEntityThreshold		= 0;
};



//----------------------------------------------------------------------------------------------------------------------
class AdminSystem
{
	friend struct GroupIter;

public:

	AdminSystem();
	AdminSystem(AdminSystemConfig const& config);

//----------------------------------------------------------------------------------------------------------------------
// BASIC API
//
public:

	void Startup();
	void Shutdown();
	
	void RunFrame(float deltaSeconds);
	void RunSystemSubgraph(SystemSubgraphID subgraphID, float deltaSeconds) const;

//----------------------------------------------------------------------------------------------------------------------
// REGISTERING COMPONENTS AND SYSTEMS (Register components/systems before calling AdminSystem::Startup())
//
public:

	void RegisterSystem(System* s, SystemSubgraphID subgraphID);

	template<typename T>
	void RegisterSystem(SystemSubgraphID subgraphID = 0);
	
	void SetSystemActive(std::string const& sysName, bool isActive) const;
	SystemSubgraph& CreateOrGetSystemSubgraph(SystemSubgraphID subgraphID);

	template <typename CType>
	void RegisterComponent(ComponentStorageType storageType = ComponentStorageType::ARRAY);

	template <typename CType>
	void RegisterResourceByType();
	
	template <typename CType>
	void RegisterComponentArray();
	
	template <typename CType>
	void RegisterComponentMap();
	
	template <typename CType>
	void RegisterTag();

	template <typename CType>
	void RegisterComponentSingleton();

private:

	void RegisterComponentBit(HashCode typeHash);

//----------------------------------------------------------------------------------------------------------------------
// CREATING/DESTROYING ENTITIES
//
public:

	EntityID CreateEntity(int searchBeginEntityID = 0);
	EntityID CreateEntityInPlace(int entityID);
	void DestroyAllEntities();
	bool DestroyEntity(EntityID entityID);



//----------------------------------------------------------------------------------------------------------------------
// ADDING COMPONENTS
//
public:

	template <typename CType, typename...Args>
	CType* AddComponent(EntityID entityID, Args const& ...args);



//----------------------------------------------------------------------------------------------------------------------
// GETTING/TRACKING COMPONENTS
//
public:

	bool DoesEntityHaveComponents(EntityID entityID, BitMask componentBitMask) const;
	bool DoesEntityExist(EntityID entityID) const;

	template <typename...CTypes>
	int Count() const;

	int NumEntities() const;



//----------------------------------------------------------------------------------------------------------------------
// GETTING COMPONENT STORAGE (use GetStorage/GetXXXStorage to pull RTTI out of system loops)
//
public:

	template <typename CType>
	CType* GetComponent(EntityID entityID = ENTITY_ID_SINGLETON) const;

	template <typename CType>
	CType& GetSingleton() const;

	BitMask GetComponentBit(HashCode componentTypeHash) const { return m_componentBitMasks.at(componentTypeHash); }

	template <typename CType>
	TypedBaseStorage<CType>* GetStorage() const;

	template <typename CType>
	ArrayStorage<CType>& GetArrayStorage() const;

	template <typename CType>
	MapStorage<CType>& GetMapStorage() const;

	template <typename CType>
	TagStorage<CType>& GetTagStorage() const;



//----------------------------------------------------------------------------------------------------------------------
// REMOVING COMPONENTS
//
public:

	template <typename CType>
	void RemoveComponent(EntityID entityID);

	void RemoveComponent(EntityID entityID, BitMask componentBit);


//----------------------------------------------------------------------------------------------------------------------
// COMPONENT ITERATION
//
public:

	template <typename...CTypes>
	GroupIter Iterate(SystemContext const& context) const;

	template <typename...CTypes>
	BitMask GetComponentBitMask() const;

	EntityID GetNextEntityWithGroup(BitMask groupMask, EntityID startIndex, EntityID endIndex) const;



//----------------------------------------------------------------------------------------------------------------------
// MISCELLANEOUS FUNCTIONS
//
public:

	std::vector<System*> GetSystems() const;
	System* GetSystemByName(std::string const& name) const;
	System* GetSystemByGlobalPriority(int globalPriority) const;

	std::vector<SystemSubgraph> const& GetSystemSubgraphs() const;

	AdminSystemConfig const& GetConfig() const;
	bool IsMultithreadingActive() const;
	void SetMultithreadingActive(bool isActive);
	
	bool IsSystemSplittingActive() const;
	void SetSystemSplittingThreshold(int newThreshold);
	
	void SetAllMultithreadingSettings(bool multithreadingActive, int autoMultithreading, int systemSplitting);
	bool IsAutoMultithreadingActive() const;
	void SetAutoMultithreadingThreshold(int newThreshold);

private:

	void RecalculateGlobalPriorities();

//----------------------------------------------------------------------------------------------------------------------
// PRIVATE DATA
//
protected:

	AdminSystemConfig				m_config;

	std::vector<SystemSubgraph>		m_systemSubgraphs;
	SystemScheduler*				m_systemScheduler = nullptr;

	BitArray<MAX_ENTITIES>			m_entities;
	BitMask							m_entityComposition[MAX_ENTITIES] = { 0 };

	std::unordered_map<HashCode, BaseStorage*>	m_componentStorage;
	std::unordered_map<HashCode, BitMask>		m_componentBitMasks;
};



//----------------------------------------------------------------------------------------------------------------------
// TEMPLATE FUNCTION IMPLEMENTATIONS																				  //
//----------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void AdminSystem::RegisterSystem(SystemSubgraphID subgraphID)
{
	System* s = new T();
	RegisterSystem(s, subgraphID);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterComponent(ComponentStorageType storageType /*= ComponentStorageType::ARRAY*/)
{
	HashCode typeHash = typeid(CType).hash_code();
	if (m_componentStorage.find(typeHash) == m_componentStorage.end())
	{
		switch (storageType)
		{
		case ComponentStorageType::MAP:
			m_componentStorage.emplace(typeHash, new MapStorage<CType>());
			break;
		case ComponentStorageType::SINGLETON:
			m_componentStorage.emplace(typeHash, new SingletonStorage<CType>());
			break;
		case ComponentStorageType::TAG:
			m_componentStorage.emplace(typeHash, new TagStorage<CType>());
			break;
		default:
			m_componentStorage.emplace(typeHash, new ArrayStorage<CType>());
			break;
		}
		RegisterComponentBit(typeHash);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterResourceByType()
{
	HashCode typeHash = typeid(CType).hash_code();
	RegisterComponentBit(typeHash);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterComponentArray()
{
	HashCode typeHash = typeid(CType).hash_code();
	if (m_componentStorage.find(typeHash) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeHash, new ArrayStorage<CType>());
		RegisterComponentBit(typeHash);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterComponentMap()
{
	HashCode typeHash = typeid(CType).hash_code();
	if (m_componentStorage.find(typeHash) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeHash, new MapStorage<CType>());
		RegisterComponentBit(typeHash);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterTag()
{
	HashCode typeHash = typeid(CType).hash_code();
	if (m_componentStorage.find(typeHash) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeHash, new TagStorage<CType>());
		RegisterComponentBit(typeHash);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterComponentSingleton()
{
	HashCode typeHash = typeid(CType).hash_code();
	if (m_componentStorage.find(typeHash) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeHash, new SingletonStorage<CType>());
		RegisterComponentBit(typeHash);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType, typename...Args>
CType* AdminSystem::AddComponent(EntityID entityID, Args const& ...args)
{
	HashCode typeHash = typeid(CType).hash_code();
	BitMask& componentBitMask = m_componentBitMasks.at(typeHash);
	
	m_entityComposition[entityID] |= (componentBitMask);

	TypedBaseStorage<CType>* typedStorage = reinterpret_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeHash));
	return typedStorage->Add(entityID, CType(args...));
}



//----------------------------------------------------------------------------------------------------------------------
// Slow (RTTI + work that can be pulled out of iterator loops).
// Use GetXXXStorage + the result's operator[] for maximum performance
//
template <typename CType>
CType* AdminSystem::GetComponent(EntityID entityID /*= ENTITY_ID_SINGLETON*/) const
{
	HashCode typeHash = typeid(CType).hash_code();
	TypedBaseStorage<CType>* typedStorage = reinterpret_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeHash));
	return typedStorage->Get(entityID);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType& AdminSystem::GetSingleton() const
{
	HashCode typeHash = typeid(CType).hash_code();
	TypedBaseStorage<CType>* typedStorage = reinterpret_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeHash));
	return *typedStorage->Get(ENTITY_ID_SINGLETON);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
TypedBaseStorage<CType>* AdminSystem::GetStorage() const
{
	HashCode typeHash = typeid(CType).hash_code();
	TypedBaseStorage<CType>* typedStorage = dynamic_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeHash));
	return typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
ArrayStorage<CType>& AdminSystem::GetArrayStorage() const
{
	HashCode typeHash = typeid(CType).hash_code();
	ArrayStorage<CType>* typedStorage = dynamic_cast<ArrayStorage<CType>*>(m_componentStorage.at(typeHash));
	return *typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
MapStorage<CType>& AdminSystem::GetMapStorage() const
{
	HashCode typeHash = typeid(CType).hash_code();
	MapStorage<CType>* typedStorage = dynamic_cast<MapStorage<CType>*>(m_componentStorage.at(typeHash));
	return *typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
TagStorage<CType>& AdminSystem::GetTagStorage() const
{
	HashCode typeHash = typeid(CType).hash_code();
	TagStorage<CType>* typedStorage = dynamic_cast<TagStorage<CType>*>(m_componentStorage.at(typeHash));
	return *typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RemoveComponent(EntityID entityID)
{
	HashCode typeHash = typeid(CType).hash_code();
	BitMask& entityComp = m_entityComposition[entityID];
	BitMask& componentBitMask = m_componentBitMasks[typeHash];
	entityComp &= (~componentBitMask);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
GroupIter AdminSystem::Iterate(SystemContext const& context) const
{
	GroupIter result(context);

	result.m_groupMask = GetComponentBitMask<CTypes...>();

	result.m_currentIndex = GetNextEntityWithGroup(result.m_groupMask, context.m_startEntityID, context.m_endEntityID);
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
BitMask AdminSystem::GetComponentBitMask() const
{
	BitMask result = 0;

	// Binary fold expression expands to:
	// 
	// result |= m_componentBitMasks.at(typeid(CTransform).hash_code()))
	//        |= m_componentBitMasks.at(typeid(CPhysics).hash_code())); 
	//        |= m_componentBitMasks.at(typeid(CMovement).hash_code()));

	(result |= ... |= m_componentBitMasks.at(typeid(CTypes).hash_code()));
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
int AdminSystem::Count() const
{
	BitMask groupMask = GetComponentBitMask<CTypes...>();

	int result = 0;
	for (EntityID i = 0; i < MAX_ENTITIES; i++)
	{
		if (m_entities.Get(i))
		{
			BitMask const& entityComp = m_entityComposition[i];
			if ((entityComp & groupMask) == groupMask)
			{
				result++;
			}
		}
	}
	return result;
}
