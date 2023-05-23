// Bradley Christensen - 2023
#include "System.h"
#include "AdminSystem.h"



//----------------------------------------------------------------------------------------------------------------------
System::System(std::string const&& name) : m_name(name)
{

}



//----------------------------------------------------------------------------------------------------------------------
bool System::DoDependenciesCollideWith(System* otherSystem) const
{
	if (m_readDependenciesBitMask & otherSystem->m_writeDependenciesBitMask)
	{
		return true;
	}
	if (m_writeDependenciesBitMask & otherSystem->m_readDependenciesBitMask)
	{
		return true;
	}
	if (m_writeDependenciesBitMask & otherSystem->m_writeDependenciesBitMask)
	{
		return true;
	}
	return false;
}



//----------------------------------------------------------------------------------------------------------------------
BitMask System::GetComponentBit(HashCode componentTypeHash) const
{
	return m_admin->GetComponentBit(componentTypeHash);
}
