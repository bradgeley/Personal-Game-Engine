// Bradley Christensen - 2023
#include "System.h"
#include "AdminSystem.h"



//----------------------------------------------------------------------------------------------------------------------
System::System(std::string const& name, Rgba8 const& debugTint) : m_name(name), m_debugTint(debugTint)
{

}



//----------------------------------------------------------------------------------------------------------------------
void System::ToggleIsActive()
{
	m_isActive = !m_isActive;
}



//----------------------------------------------------------------------------------------------------------------------
Rgba8 const& System::GetDebugTint() const
{
	return m_debugTint;
}



//----------------------------------------------------------------------------------------------------------------------
BitMask const& System::GetReadDependencies() const
{
	return m_readDependenciesBitMask;
}



//----------------------------------------------------------------------------------------------------------------------
BitMask const& System::GetWriteDependencies() const
{
	return m_writeDependenciesBitMask;
}



//----------------------------------------------------------------------------------------------------------------------
void System::AddWriteAllDependencies()
{
	m_writeDependenciesBitMask = SIZE_MAX;
}