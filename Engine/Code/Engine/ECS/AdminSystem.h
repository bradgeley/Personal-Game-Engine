// Bradley Christensen - 2022-2026
#pragma once
#include "ComponentStorage.h"
#include "GroupIter.h"
#include "EntityID.h"
#include "SystemSubgraph.h"
#include "SystemContext.h"
#include "Engine/Core/Name.h"
#include <vector>
#include <unordered_map>
#include <typeindex>



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
// LIFECYCLE
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
	
	void SetSystemActive(Name systemName, bool isActive) const;
	SystemSubgraph& CreateOrGetSystemSubgraph(SystemSubgraphID subgraphID);

	template <typename CType>
	void RegisterComponent(ComponentStorageType storageType = ComponentStorageType::Array);

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

	void RegisterComponentBit(std::type_index typeIndex);

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

	template <typename CType>
	CType* AddComponent(EntityID entityID, CType const& copy);



//----------------------------------------------------------------------------------------------------------------------
// GETTING/TRACKING COMPONENTS
//
public:

	template<typename CType>
	bool HasComponent(EntityID entityID) const;

	template<typename CType>
	bool HasComponentUnsafe(int entityIndex) const;

	bool DoesEntityHaveComponents(EntityID entityID, BitMask componentBitMask) const;
	bool DoesEntityHaveComponentsUnsafe(int entityIndex, BitMask componentBitMask) const;

	bool IsValid(EntityID entityID) const;

	template <typename...CTypes>
	int Count() const;

	int NumEntities() const;



//----------------------------------------------------------------------------------------------------------------------
// GETTING COMPONENT STORAGE (use GetStorage/GetXXXStorage to pull RTTI out of system loops)
//
public:

	template <typename CType>
	CType* GetComponent(EntityID entityID = EntityID::Singleton) const;

	template <typename CType>
	inline CType* GetComponent(GroupIter const& it) const;

	template <typename CType>
	CType& GetSingleton() const;

	inline BitMask GetComponentBit(std::type_index type) const { return m_componentBitMasks.at(type); }

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
	GroupIter IterateAll() const;

	template <typename...CTypes>
	GroupIter Iterate(SystemContext const& context) const;

	template <typename...CTypes>
	BitMask GetComponentBitMask() const;

	EntityID GetNextEntityWithGroup(BitMask groupMask, int startIndex, int endIndex) const;
	int GetNextEntityIndexWithGroup(BitMask groupMask, int startIndex, int endIndex) const;



//----------------------------------------------------------------------------------------------------------------------
// MISCELLANEOUS FUNCTIONS
//
public:

	System* GetSystemByName(Name name) const;
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
	BitMask							m_entityComposition[MAX_ENTITIES] = { 0 }; // Todo: if user needs more than 32 or 64 components, allow them to use a fixed size BitArray for entity composition, so the max component count would be uncapped
	uint16_t						m_entityGeneration[MAX_ENTITIES] = { 0 }; // 8 bit to match the 8 bits inside of EntityID
	int								m_highWatermarkEntityID = 0; // highest entity ID ever allocated, speeds up system iteration by only searching up to this ID

	std::unordered_map<std::type_index, BaseStorage*>	m_componentStorage;
	std::unordered_map<std::type_index, BitMask>		m_componentBitMasks;
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
void AdminSystem::RegisterComponent(ComponentStorageType storageType /*= ComponentStorageType::Array*/)
{
	std::type_index typeIndex(typeid(CType));
	if (m_componentStorage.find(typeIndex) == m_componentStorage.end())
	{
		switch (storageType)
		{
		case ComponentStorageType::Map:
			m_componentStorage.emplace(typeIndex, new MapStorage<CType>());
			break;
		case ComponentStorageType::Singleton:
			m_componentStorage.emplace(typeIndex, new SingletonStorage<CType>());
			break;
		case ComponentStorageType::Tag:
			m_componentStorage.emplace(typeIndex, new TagStorage<CType>());
			break;
		default:
			m_componentStorage.emplace(typeIndex, new ArrayStorage<CType>());
			break;
		}
		RegisterComponentBit(typeIndex);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterResourceByType()
{
	std::type_index typeIndex(typeid(CType));
	RegisterComponentBit(typeIndex);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterComponentArray()
{
	std::type_index typeIndex(typeid(CType));
	if (m_componentStorage.find(typeIndex) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeIndex, new ArrayStorage<CType>());
		RegisterComponentBit(typeIndex);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterComponentMap()
{
	std::type_index typeIndex(typeid(CType));
	if (m_componentStorage.find(typeIndex) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeIndex, new MapStorage<CType>());
		RegisterComponentBit(typeIndex);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterTag()
{
	std::type_index typeIndex(typeid(CType));
	if (m_componentStorage.find(typeIndex) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeIndex, new TagStorage<CType>());
		RegisterComponentBit(typeIndex);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RegisterComponentSingleton()
{
	std::type_index typeIndex(typeid(CType));
	if (m_componentStorage.find(typeIndex) == m_componentStorage.end())
	{
		m_componentStorage.emplace(typeIndex, new SingletonStorage<CType>());
		RegisterComponentBit(typeIndex);
	}
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType, typename...Args>
CType* AdminSystem::AddComponent(EntityID entityID, Args const& ...args)
{
	if (!IsValid(entityID))
	{
		return nullptr;
	}

	int entityIndex = entityID.GetIndex();
	std::type_index typeIndex(typeid(CType));
	BitMask& componentBitMask = m_componentBitMasks.at(typeIndex);
	TypedBaseStorage<CType>* typedStorage = reinterpret_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeIndex));

	if (HasComponentUnsafe<CType>(entityIndex))
	{
		return typedStorage->Get(entityIndex);
	}

	m_entityComposition[entityIndex] |= (componentBitMask);
	return typedStorage->Add(entityIndex, CType(args...));
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType* AdminSystem::AddComponent(EntityID entityID, CType const& copy)
{
	if (!IsValid(entityID))
	{
		return nullptr;
	}

	int entityIndex = entityID.GetIndex();
	std::type_index typeIndex(typeid(CType));
	BitMask& componentBitMask = m_componentBitMasks.at(typeIndex);
	TypedBaseStorage<CType>* typedStorage = reinterpret_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeIndex));

	if (HasComponentUnsafe<CType>(entityIndex))
	{
		return typedStorage->Get(entityIndex);
	}

	m_entityComposition[entityIndex] |= (componentBitMask);

	return typedStorage->Add(entityIndex, copy);
}



//----------------------------------------------------------------------------------------------------------------------
// Slow (RTTI + work that can be pulled out of iterator loops).
// Use GetXXXStorage + operator[GroupIter] for maximum performance
//
template <typename CType>
CType* AdminSystem::GetComponent(EntityID entityID /*= EntityID::Singleton*/) const
{
	if (!IsValid(entityID))
	{
		return nullptr;
	}

	int entityIndex = entityID.GetIndex();
	if (!HasComponentUnsafe<CType>(entityIndex))
	{
		return nullptr;
	}

	std::type_index typeIndex(typeid(CType));
	TypedBaseStorage<CType>* typedStorage = reinterpret_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeIndex));
	return typedStorage->Get(entityIndex);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename CType>
inline CType* AdminSystem::GetComponent(GroupIter const& it) const
{
	return GetComponent<CType>(it.GetEntityID());
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
CType& AdminSystem::GetSingleton() const
{
	std::type_index typeIndex(typeid(CType));
	TypedBaseStorage<CType>* typedStorage = reinterpret_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeIndex));
	return *typedStorage->Get(0);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
TypedBaseStorage<CType>* AdminSystem::GetStorage() const
{
	std::type_index typeIndex(typeid(CType));
	TypedBaseStorage<CType>* typedStorage = dynamic_cast<TypedBaseStorage<CType>*>(m_componentStorage.at(typeIndex));
	return typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
ArrayStorage<CType>& AdminSystem::GetArrayStorage() const
{
	std::type_index typeIndex(typeid(CType));
	ArrayStorage<CType>* typedStorage = dynamic_cast<ArrayStorage<CType>*>(m_componentStorage.at(typeIndex));
	return *typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
MapStorage<CType>& AdminSystem::GetMapStorage() const
{
	std::type_index typeIndex(typeid(CType));
	MapStorage<CType>* typedStorage = dynamic_cast<MapStorage<CType>*>(m_componentStorage.at(typeIndex));
	return *typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
TagStorage<CType>& AdminSystem::GetTagStorage() const
{
	std::type_index typeIndex(typeid(CType));
	TagStorage<CType>* typedStorage = dynamic_cast<TagStorage<CType>*>(m_componentStorage.at(typeIndex));
	return *typedStorage;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename CType>
void AdminSystem::RemoveComponent(EntityID entityID)
{
	if (!IsValid(entityID))
	{
		return;
	}
	std::type_index typeIndex(typeid(CType));
	int entityIndex = entityID.GetIndex();
	BitMask& entityComp = m_entityComposition[entityIndex];
	BitMask& componentBitMask = m_componentBitMasks[typeIndex];
	entityComp &= (~componentBitMask);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
GroupIter AdminSystem::IterateAll() const
{
	GroupIter result;

	result.m_groupMask = GetComponentBitMask<CTypes...>();

	result.m_currentIndex = GetNextEntityIndexWithGroup(result.m_groupMask, 0, MAX_ENTITIES - 1);

	if (m_highWatermarkEntityID < result.m_endIndex)
	{
		result.m_endIndex = g_ecs->m_highWatermarkEntityID;
	}

	return result;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
GroupIter AdminSystem::Iterate(SystemContext const& context) const
{
	GroupIter result(context);

	if (m_highWatermarkEntityID < result.m_endIndex)
	{
		result.m_endIndex = g_ecs->m_highWatermarkEntityID;
	}

	result.m_groupMask = GetComponentBitMask<CTypes...>();

	result.m_currentIndex = GetNextEntityIndexWithGroup(result.m_groupMask, context.m_startEntityID, context.m_endEntityID);
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
BitMask AdminSystem::GetComponentBitMask() const
{
	BitMask result = 0;

	// Binary fold expression expands to:
	// 
	// result |= m_componentBitMasks.at(type_index(typeid(CType1)))
	//        |= m_componentBitMasks.at(type_index(typeid(CType2))); 
	//        |= m_componentBitMasks.at(type_index(typeid(CType3)));

	(result |= ... |= m_componentBitMasks.at(std::type_index(typeid(CTypes))));
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
template<typename CType>
inline bool AdminSystem::HasComponent(EntityID entityID) const
{
	if (!IsValid(entityID))
	{
		return false;
	}
	BitMask bitMask = GetComponentBit(std::type_index(typeid(CType)));
	return DoesEntityHaveComponents(entityID, bitMask);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename CType>
inline bool AdminSystem::HasComponentUnsafe(int entityIndex) const
{
	BitMask bitMask = GetComponentBit(std::type_index(typeid(CType)));
	return DoesEntityHaveComponentsUnsafe(entityIndex, bitMask);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename...CTypes>
int AdminSystem::Count() const
{
	BitMask groupMask = GetComponentBitMask<CTypes...>();

	int result = 0;
	for (int i = 0; i < MAX_ENTITIES; i++)
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
