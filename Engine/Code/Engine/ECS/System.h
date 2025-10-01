// Bradley Christensen - 2022-2025
#pragma once
#include "AdminSystem.h"
#include "SystemContext.h"
#include "Engine/Renderer/Rgba8.h" // <-- may want to remove outside dependencies
#include "Engine/Core/Name.h" // <-- may want to remove outside dependencies
#include "Config.h"
#include <typeindex>



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

	explicit System(Name name = "Unnamed System", Rgba8 const& debugTint = Rgba8::Magenta);
	virtual ~System() = default;

	virtual void Startup()											{}  // Main thread, once per system
	virtual void BeginFrame()										{}  // Main thread, once per system, before any systems run
	virtual void PreRun()											{}	// Main thread, once per system
	virtual void Run(SystemContext const&)							{}  // Worker threads, depending on system splitting settings
	virtual void PostRun()											{}  // Main thread, once per system
	virtual void EndFrame()											{}  // Main thread, once per system, after all systems run
	virtual void Shutdown()											{}  // Main thread, once per system

	bool IsActive() const											{ return m_isActive; }
	void SetActive(bool isActive)									{ m_isActive = isActive; }
	void ToggleIsActive();
	void SetLocalPriority(int localPriority)						{ m_localPriority = localPriority; }
	void SetGlobalPriority(int globalPriority)						{ m_globalPriority = globalPriority; }
	int	 GetLocalPriority() const									{ return m_localPriority; }
	int	 GetGlobalPriority() const									{ return m_globalPriority; }
	int	 GetSystemSplittingNumJobs() const							{ return m_systemSplittingNumJobs; }
	void SetSystemSplittingNumJobs(int numThreads)					{ m_systemSplittingNumJobs = numThreads; }
	Name GetName() const											{ return m_name; }
	Rgba8 const& GetDebugTint() const;

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

	void AddWriteAllDependencies();

protected:

	Name				m_name						= "Unnamed System";
	Rgba8				m_debugTint					= Rgba8::Magenta;
	bool				m_isActive					= true;
	int					m_systemSplittingNumJobs	= 1; // 0-1 means do not split the system

	int					m_localPriority				= -1;
	int					m_globalPriority			= -1;
	BitMask				m_readDependenciesBitMask	= 0;
	BitMask				m_writeDependenciesBitMask	= 0;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename CType>
void System::AddResourceDependency(AccessType access)
{
	std::type_index typeIndex = std::type_index(typeid(CType));
	BitMask componentBit = g_ecs->GetComponentBit(typeIndex); // all used components must be registered before this...

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
	m_writeDependenciesBitMask |= g_ecs->GetComponentBitMask<CTypes...>();
}



//----------------------------------------------------------------------------------------------------------------------
template<typename...CTypes>
void System::AddReadDependencies()
{
	m_readDependenciesBitMask |= g_ecs->GetComponentBitMask<CTypes...>();
}
