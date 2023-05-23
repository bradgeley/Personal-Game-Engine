// Bradley Christensen - 2023
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Core/ErrorUtils.h"
#include "AdminSystem.h"
#include "SystemScheduler.h"
#include "System.h"



//----------------------------------------------------------------------------------------------------------------------
AdminSystem::AdminSystem(AdminSystemConfig const& config) : m_config(config), m_entities(false)
{

}



//----------------------------------------------------------------------------------------------------------------------
AdminSystem::AdminSystem() : m_config(), m_entities(false)
{

}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::Startup()
{
	m_systemScheduler = new SystemScheduler(this);

	for (System* s : m_systems)
	{
		s->Startup();
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RunSystems(float deltaSeconds) const
{
	if (deltaSeconds > m_config.m_maxDeltaSeconds)
	{
		deltaSeconds = m_config.m_maxDeltaSeconds;
	}

	m_systemScheduler->Schedule(m_systems);
	
	if (IsAutoMultithreadingActive())
	{
		m_systemScheduler->RunMultithreaded(deltaSeconds);
	}
	else
	{
		m_systemScheduler->RunSinglethreaded(deltaSeconds);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::Shutdown()
{
	for (System* s : m_systems)
	{
		s->Shutdown();
		delete s;
	}

	for (auto it = m_componentStorage.begin(); it != m_componentStorage.end(); ++it)
	{
		delete it->second;
	}
	
	delete m_systemScheduler;
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
std::vector<System*> const& AdminSystem::GetSystems() const
{
	return m_systems;
}



//----------------------------------------------------------------------------------------------------------------------
System* AdminSystem::GetSystemByName(std::string const& name) const
{
	for (auto& s : m_systems)
	{
		if (s->GetName() == name)
		{
			return s;
		}
	}
	return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<System*> AdminSystem::GetSystemsByPriority() const
{
	std::vector<System*> result;
	result.resize(m_systems.size());
	int ignorePrio = -1;
	int lowestPrio = INT_MAX;
	System* lowestPrioSystem = nullptr;

	for (int i = 0; i < (int) m_systems.size(); ++i)
	{
		for (auto s : m_systems)
		{
			int prio = s->GetPriority();
			if (prio < lowestPrio && prio > ignorePrio)
			{
				lowestPrio = prio;
				lowestPrioSystem = s;
			}
		}
		result[i] = lowestPrioSystem;
		ignorePrio = lowestPrio;
		lowestPrio = INT_MAX;
	}

	return result;
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
void AdminSystem::RegisterSystem(System* s)
{
	s->m_admin = this;
	if (s->GetPriority() == -1)
	{
		s->SetRunPrio((int) m_systems.size());
	}
	m_systems.push_back(s);
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::SetSystemActive(std::string const& sysName, bool isActive) const
{
	for (auto& s : m_systems)
	{
		if (s->GetName() == sysName)
		{
			s->SetActive(isActive);
			return;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void AdminSystem::RegisterComponentBit(HashCode typeHash)
{
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
void AdminSystem::ClearEntities()
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