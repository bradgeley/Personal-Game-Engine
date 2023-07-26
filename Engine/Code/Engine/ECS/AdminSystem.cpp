// Bradley Christensen - 2023
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Core/ErrorUtils.h"
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

	for (auto& s : m_systemSubgraphs)
	{
		s.Startup();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::Shutdown()
{
	for (auto& s : m_systemSubgraphs)
	{
		s.Shutdown();
		s.Cleanup();
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
void AdminSystem::RunFrame(float deltaSeconds) const
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
	m_entityComposition[entityID] = 0i64;
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
std::vector<System*> AdminSystem::GetSystems() const
{
	std::vector<System*> result;
	result.reserve(100);
	for (auto& subgraph : m_systemSubgraphs)
	{
		result.insert(result.end(), subgraph.m_systems.begin(), subgraph.m_systems.end());
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
System* AdminSystem::GetSystemByName(std::string const& name) const
{
	for (auto& subgraph : m_systemSubgraphs)
	{
		for (auto& system : subgraph.m_systems)
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
void AdminSystem::RegisterSystem(System* s, SystemSubgraphID subgraphID)
{
	if (subgraphID >= m_systemSubgraphs.size())
	{
		m_systemSubgraphs.resize(subgraphID + 1);
	}

	if (s->GetPriority() == -1)
	{
		s->SetRunPrio((int) m_systemSubgraphs[subgraphID].m_systems.size());
	}
	
	m_systemSubgraphs[subgraphID].m_systems.emplace_back(s);
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::SetSystemActive(std::string const& sysName, bool isActive) const
{
	System* s = GetSystemByName(sysName);
	if (s)
	{
		s->SetActive(isActive);
	}
}



//----------------------------------------------------------------------------------------------------------------------
SystemSubgraph& AdminSystem::GetSystemSubgraph(SystemSubgraphID subgraphID)
{
	if (subgraphID >= m_systemSubgraphs.size())
	{
		m_systemSubgraphs.resize(subgraphID + 1);
	}
	return m_systemSubgraphs[subgraphID];
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RegisterComponentBit(HashCode typeHash)
{
	if (m_componentBitMasks.find(typeHash) != m_componentBitMasks.end())
	{
		// Already registered
		return;
	}
	
	ASSERT_OR_DIE(m_componentBitMasks.size() < 64, "Max number of components reached.")

	size_t componentIndex = m_componentBitMasks.size();
	BitMask bitMask = (1i64 << componentIndex);
	m_componentBitMasks.emplace(typeHash, bitMask);
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
	for (auto& entityComp : m_entityComposition)
	{
		entityComp = 0i64;
	}
	for (auto& storage : m_componentStorage)
	{
		auto& baseStorage = storage.second;
		baseStorage->Clear();
	}
}