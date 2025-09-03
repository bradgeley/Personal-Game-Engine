// Bradley Christensen - 2022-2025
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "AdminSystem.h"
#include "Component.h"
#include "SystemScheduler.h"
#include "System.h"



//----------------------------------------------------------------------------------------------------------------------
// THE ecs
//
AdminSystem* g_ecs = nullptr;



//----------------------------------------------------------------------------------------------------------------------
AdminSystem::AdminSystem(AdminSystemConfig const& config) : m_config(config), m_entities(false)
{

}



//----------------------------------------------------------------------------------------------------------------------
AdminSystem::AdminSystem() : m_entities(false)
{

}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::Startup()
{
	m_systemScheduler = new SystemScheduler(this);

	for (SystemSubgraph& subgraph : m_systemSubgraphs)
	{
		subgraph.Startup();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::Shutdown()
{
	for (SystemSubgraph& subgraph : m_systemSubgraphs)
	{
		subgraph.Shutdown();
		subgraph.Cleanup();
	}

	m_systemSubgraphs.clear();

	for (auto it = m_componentStorage.begin(); it != m_componentStorage.end(); ++it)
	{
		delete it->second;
	}

	m_componentStorage.clear();
	m_componentBitMasks.clear();
	
	delete m_systemScheduler;
	m_systemScheduler = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RunFrame(float deltaSeconds)
{
	if (m_config.m_maxDeltaSeconds > 0.f && deltaSeconds > m_config.m_maxDeltaSeconds)
	{
		deltaSeconds = m_config.m_maxDeltaSeconds;
	}

	m_systemScheduler->ScheduleFrame(m_systemSubgraphs);
	m_systemScheduler->RunFrame(deltaSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RunSystemSubgraph(SystemSubgraphID subgraphID, float deltaSeconds) const
{
	if ((int) subgraphID >= (int) m_systemSubgraphs.size() || (int)subgraphID < 0)
	{
		return;
	}
	
	if (m_config.m_maxDeltaSeconds > 0.f && deltaSeconds > m_config.m_maxDeltaSeconds)
	{
		deltaSeconds = m_config.m_maxDeltaSeconds;
	}

	m_systemScheduler->RunSubgraph(m_systemSubgraphs[subgraphID], deltaSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::DestroyEntity(EntityID entityID)
{
	m_entities.Unset((int) entityID);
	m_entityComposition[entityID] = (BitMask) 0;
	for (auto it = m_componentStorage.begin(); it != m_componentStorage.end(); ++it)
	{
		BaseStorage* storage = it->second;
		storage->Destroy(entityID); // Singleton storage ignores destroying
	}
	return true;
}



//----------------------------------------------------------------------------------------------------------------------
int AdminSystem::NumEntities() const
{
	return m_entities.CountSetBits();
}



//----------------------------------------------------------------------------------------------------------------------
System* AdminSystem::GetSystemByName(Name name) const
{
	for (SystemSubgraph const& subgraph : m_systemSubgraphs)
	{
		for (System* const& system : subgraph.m_systems)
		{
			if (system->GetName() == name)
			{
				return system;
			}
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
System* AdminSystem::GetSystemByGlobalPriority(int globalPriority) const
{
	for (SystemSubgraph const& subgraph : m_systemSubgraphs)
	{
		for (System* const& system : subgraph.m_systems)
		{
			if (system->GetGlobalPriority() == globalPriority)
			{
				return system;
			}
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<SystemSubgraph> const& AdminSystem::GetSystemSubgraphs() const
{
	return m_systemSubgraphs;
}



//----------------------------------------------------------------------------------------------------------------------
AdminSystemConfig const& AdminSystem::GetConfig() const
{
	return m_config;
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::IsMultithreadingActive() const
{
	return m_config.m_enableMultithreading;
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::SetMultithreadingActive(bool isActive)
{
	m_config.m_enableMultithreading = isActive;
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::IsSystemSplittingActive() const
{
	if (IsMultithreadingActive())
	{
		if (m_config.m_systemSplittingEntityThreshold > 0)
		{
			return NumEntities() > m_config.m_systemSplittingEntityThreshold;
		}
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::SetSystemSplittingThreshold(int newThreshold)
{
	m_config.m_systemSplittingEntityThreshold = newThreshold;
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::SetAllMultithreadingSettings(bool multithreadingActive, int autoMultithreading, int systemSplitting)
{
	SetMultithreadingActive(multithreadingActive);
	SetAutoMultithreadingThreshold(autoMultithreading);
	SetSystemSplittingThreshold(systemSplitting);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::IsAutoMultithreadingActive() const
{
	if (IsMultithreadingActive())
	{
		if (m_config.m_autoMultithreadingEntityThreshold > 0)
		{
			return NumEntities() >= m_config.m_autoMultithreadingEntityThreshold;
		}
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::SetAutoMultithreadingThreshold(int newThreshold)
{
	m_config.m_autoMultithreadingEntityThreshold = newThreshold;
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RecalculateGlobalPriorities()
{
	int priority = 0;
	for (SystemSubgraph& systemSubgraph : m_systemSubgraphs)
	{
		for (System*& system : systemSubgraph.m_systems)
		{
			system->SetGlobalPriority(priority);
			++priority;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RegisterSystem(System* s, SystemSubgraphID subgraphID)
{
	if (subgraphID >= m_systemSubgraphs.size())
	{
		m_systemSubgraphs.resize(subgraphID + 1);
	}

	if (s->GetLocalPriority() == -1)
	{
		s->SetLocalPriority((int) m_systemSubgraphs[subgraphID].m_systems.size());
	}
	
	m_systemSubgraphs[subgraphID].m_systems.emplace_back(s);

	RecalculateGlobalPriorities();
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::SetSystemActive(Name systemName, bool isActive) const
{
	System* system = GetSystemByName(systemName);
	if (system)
	{
		system->SetActive(isActive);
	}
}



//----------------------------------------------------------------------------------------------------------------------
SystemSubgraph& AdminSystem::CreateOrGetSystemSubgraph(SystemSubgraphID subgraphID)
{
	if (subgraphID >= m_systemSubgraphs.size())
	{
		m_systemSubgraphs.resize(subgraphID + 1);
	}
	return m_systemSubgraphs[subgraphID];
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RegisterComponentBit(std::type_index typeIndex)
{
	if (m_componentBitMasks.find(typeIndex) != m_componentBitMasks.end())
	{
		// Already registered
		return;
	}
	
	ASSERT_OR_DIE(m_componentBitMasks.size() < 64, "Max number of components reached.")

	size_t componentIndex = m_componentBitMasks.size();
	BitMask bitMask = ((BitMask) 1 << componentIndex);
	m_componentBitMasks.emplace(typeIndex, bitMask);
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RemoveComponent(EntityID entityID, BitMask componentBit)
{
	BitMask& entityComp = m_entityComposition[entityID];
	entityComp &= (~componentBit);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::DoesEntityHaveComponents(EntityID entityID, BitMask componentBitMask) const
{
	return componentBitMask == (m_entityComposition[entityID] & componentBitMask);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::DoesEntityExist(EntityID entityID) const
{
	return m_entities.Get((int) entityID);
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AdminSystem::GetNextEntityWithGroup(BitMask groupMask, EntityID startIndex, EntityID endIndex) const
{
	for (EntityID entity = startIndex; entity <= endIndex; ++entity)
	{
		if ((m_entityComposition[entity] & groupMask) == groupMask)
		{
			return entity;
		}
	}
	return UINT_MAX;
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AdminSystem::CreateEntity(int searchBeginEntityID)
{
	// search bit array for open index
	int index = m_entities.SetNextUnsetIndex(searchBeginEntityID);

	// check if we're at max entities
	if (index == -1)
	{
		return ENTITY_ID_INVALID;
	}

	return (EntityID) index;
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AdminSystem::CreateEntityInPlace(int entityID)
{
	m_entities.Set(entityID);

	return (EntityID) entityID;
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::DestroyAllEntities()
{
	m_entities.SetAll(false);
	for (BitMask& entityComp : m_entityComposition)
	{
		entityComp = (BitMask) 0;
	}
	for (auto& storage : m_componentStorage)
	{
		auto& baseStorage = storage.second;
		baseStorage->Clear();
	}
}