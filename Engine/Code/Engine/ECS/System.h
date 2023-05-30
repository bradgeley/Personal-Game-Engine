// Bradley Christensen - 2023
#pragma once
#include "AdminSystem.h"
#include "SystemContext.h"
#include "Config.h"
#include <string>
#include <typeinfo>



class AdminSystem;



//----------------------------------------------------------------------------------------------------------------------
enum class AccessType
{
	READ,
	WRITE
};



//----------------------------------------------------------------------------------------------------------------------
// System
//
// One pass of logic over the game.
//
class System
{
	friend class AdminSystem;

public:

	explicit System(std::string const&& name = "Unnamed System");
	virtual ~System() = default;

	virtual void Startup()											{}
	virtual void PreRun()											{}
	virtual void Run(SystemContext const& [[maybe_unused]] context)	{}
	virtual void PostRun()											{}
	virtual void Shutdown()											{}

	bool IsActive() const											{ return m_isActive; }
	void SetActive(bool isActive)									{ m_isActive = isActive; }
	void SetRunPrio(int prio)										{ m_priority = prio; }
	int	 GetPriority() const										{ return m_priority; }
	int	 GetSystemSplittingNumJobs() const							{ return m_systemSplittingNumJobs; }
	void SetSystemSplittingNumJobs(int numThreads)					{ m_systemSplittingNumJobs = numThreads; }
	std::string const& GetName() const								{ return m_name; }

	template<typename...CTypes>
	GroupIter Iterate(SystemContext const& context) const;

	BitMask const& GetReadDependencies() const; 
	BitMask const& GetWriteDependencies() const;

protected:

	// Call these during system creation or startup, but AFTER registering component types to the admin system
	template<typename CType>
	void AddResourceDependency(AccessType access);

	template<typename...CTypes>
	void AddWriteDependencies();

	template<typename...CTypes>
	void AddReadDependencies();

protected:

	std::string const	m_name						= "Unnamed System";
	AdminSystem*		m_admin						= nullptr;
	bool				m_isActive					= true;
	int					m_systemSplittingNumJobs	= 1; // 0-1 means do not split the system

	int					m_priority					= -1;
	BitMask				m_readDependenciesBitMask	= 0;
	BitMask				m_writeDependenciesBitMask	= 0;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename ... CTypes>
GroupIter System::Iterate(SystemContext const& context) const
{
	return m_admin->Iterate<CTypes...>(context);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename CType>
void System::AddResourceDependency(AccessType access)
{
	HashCode typeHash = typeid(CType).hash_code();
	BitMask componentBit = m_admin->GetComponentBit(typeHash); // all used components must be registered before this...

	if (access == AccessType::WRITE)
	{
		m_writeDependenciesBitMask |= componentBit;
	}
	else
	{
		m_readDependenciesBitMask |= componentBit;
	}
}



//----------------------------------------------------------------------------------------------------------------------
template<typename...CTypes>
void System::AddWriteDependencies()
{
	m_writeDependenciesBitMask |= m_admin->GetComponentBitMask<CTypes...>();
}



//----------------------------------------------------------------------------------------------------------------------
template<typename...CTypes>
void System::AddReadDependencies()
{
	m_readDependenciesBitMask |= m_admin->GetComponentBitMask<CTypes...>();
}
