// Bradley Christensen - 2022-2026
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

	DestroyAllEntities();

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
	if (!IsValid(entityID))
	{
		return false;
	}

	int entityIndex = entityID.GetIndex();
	m_entities.Unset(entityIndex);
	m_entityComposition[entityIndex] = (BitMask) 0;
	m_entityGeneration[entityIndex] = (m_entityGeneration[entityIndex] + 1) & ENTITY_GENERATION_MASK;

	for (auto it = m_componentStorage.begin(); it != m_componentStorage.end(); ++it)
	{
		BaseStorage* storage = it->second;
		storage->Destroy(entityIndex); // Singleton storage ignores destroying
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
int AdminSystem::GetNumRegisteredComponents() const
{
	return static_cast<int>(m_componentBitMasks.size());
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RegisterComponentBit(std::type_index typeIndex)
{
	if (m_componentBitMasks.find(typeIndex) != m_componentBitMasks.end())
	{
		// Already registered
		return;
	}
	
	constexpr int maxComponents = sizeof(size_t) * 8;
	ASSERT_OR_DIE(m_componentBitMasks.size() < maxComponents, "Max number of components reached.")

	size_t componentIndex = m_componentBitMasks.size();
	BitMask bitMask = ((BitMask) 1 << componentIndex);
	m_componentBitMasks.emplace(typeIndex, bitMask);
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RemoveComponent(EntityID entityID, BitMask componentBit)
{
	if (!IsValid(entityID))
	{
		return;
	}

	BitMask& entityComp = m_entityComposition[entityID.GetIndex()];
	entityComp &= (~componentBit);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::DoesEntityHaveComponents(EntityID entityID, BitMask componentBitMask) const
{
	if (!IsValid(entityID))
	{
		return false;
	}

	return componentBitMask == (m_entityComposition[entityID.GetIndex()] & componentBitMask);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::DoesEntityHaveComponentsUnsafe(int entityIndex, BitMask componentBitMask) const
{
	return componentBitMask == (m_entityComposition[entityIndex] & componentBitMask);
}



//----------------------------------------------------------------------------------------------------------------------
bool AdminSystem::IsValid(EntityID entityID) const
{
	int entityIndex = entityID.GetIndex();
	if (entityIndex >= MAX_ENTITIES)
	{
		return false;
	}

	if (m_entityGeneration[entityIndex] != entityID.GetGeneration())
	{
		return false;
	}

	return m_entities.Get(entityIndex);
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AdminSystem::GetNextEntityWithGroup(BitMask groupMask, int startIndex, int endIndex) const
{
	int lastIndex = m_highWatermarkEntityID < static_cast<int>(endIndex) ? m_highWatermarkEntityID : static_cast<int>(endIndex);

	for (int entity = startIndex; entity <= lastIndex; ++entity)
	{
		if ((m_entityComposition[entity] & groupMask) == groupMask)
		{
			return EntityID(entity, m_entityGeneration[entity]);
		}
	}
	return EntityID::Invalid;
}



//----------------------------------------------------------------------------------------------------------------------
int AdminSystem::GetNextEntityIndexWithGroup(BitMask groupMask, int startIndex, int endIndex) const
{
	int lastIndex = m_highWatermarkEntityID < static_cast<int>(endIndex) ? m_highWatermarkEntityID : static_cast<int>(endIndex);

	for (int entity = startIndex; entity <= lastIndex; ++entity)
	{
		if ((m_entityComposition[entity] & groupMask) == groupMask)
		{
			return entity;
		}
	}
	return INT_MAX;
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AdminSystem::CreateEntity(int searchBeginEntityID)
{
	// search bit array for open index
	int index = m_entities.SetNextUnsetIndex(searchBeginEntityID);

	// check if we're at max entities
	if (index == -1)
	{
		return EntityID::Invalid;
	}

	if (index > m_highWatermarkEntityID)
	{
		m_highWatermarkEntityID = index;
	}

	return EntityID(index, m_entityGeneration[index]);
}



//----------------------------------------------------------------------------------------------------------------------
EntityID AdminSystem::CreateEntityInPlace(int entityID)
{
	if (m_entities.Get(entityID))
	{
		return EntityID::Invalid;
	}

	m_entities.Set(entityID);

	if (entityID > m_highWatermarkEntityID)
	{
		m_highWatermarkEntityID = entityID;
	}

	return EntityID(entityID, m_entityGeneration[entityID]);
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::DestroyAllEntities()
{
	for (int i = 0; i < MAX_ENTITIES; ++i)
	{
		DestroyEntity(EntityID(i, m_entityGeneration[i]));
	}

	m_highWatermarkEntityID = 0;

}