// Bradley Christensen - 2023
#include "System.h"
#include "AdminSystem.h"



//----------------------------------------------------------------------------------------------------------------------
System::System(std::string const&& name) : m_name(name)
{

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