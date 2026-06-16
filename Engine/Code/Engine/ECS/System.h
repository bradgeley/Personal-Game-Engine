// Bradley Christensen - 2022-2026
#pragma once
#include "AdminSystem.h"
#include "SystemContext.h"
#include "Engine/Renderer/Rgba8.h"
#include "Engine/Core/Name.h"
#include "Config.h"



class AdminSystem;



//----------------------------------------------------------------------------------------------------------------------
// System
//
// One pass of logic over the game
//
class System
{
	friend class AdminSystem;

public:

	explicit System(Name name = "Unnamed System", Rgba8 const& debugTint = Rgba8::Magenta);
	virtual ~System() = default;

	virtual void Startup()											{}  // Main thread, once per system
	virtual void Shutdown() const									{}  // Main thread, once per system

	virtual void BeginFrame() const									{}  // Main thread, once per system, before any systems run
	virtual void PreRun(SystemContext const&) const					{}	// Workers or main, once per system
	virtual void Run(SystemContext const&) const					{}  // Workers or main, may be more than 1 thread when splitting
	virtual void PostRun(SystemContext const&) const				{}  // Workers or main, once per system
	virtual void EndFrame() const									{}  // Main thread, once per system, after all systems run

	bool IsActive() const											{ return m_isActive; }
	void SetActive(bool isActive)									{ m_isActive = isActive; }
	bool ShouldRun() const											{ return m_isActive && !m_ignoreRun; }
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

	template<typename...CTypes>
	void AddWriteDependencies();

	template<typename...CTypes>
	void AddReadDependencies();

	void AddWriteAllDependencies();

protected:

	Name				m_name						= "Unnamed System";
	Rgba8				m_debugTint					= Rgba8::Magenta;
	bool				m_isActive					= true;
	bool				m_ignoreRun					= false;
	int					m_systemSplittingNumJobs	= 1; // 0-1 means do not split the system

	int					m_localPriority				= -1;
	int					m_globalPriority			= -1;
	BitMask				m_readDependenciesBitMask	= 0;
	BitMask				m_writeDependenciesBitMask	= 0;
};



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
