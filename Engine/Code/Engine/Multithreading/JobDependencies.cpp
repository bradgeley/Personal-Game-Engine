// Bradley Christensen - 2023
#include "JobDependencies.h"



//----------------------------------------------------------------------------------------------------------------------
bool JobDependencies::SharesDependencies(JobDependencies const& other) const
{
    if ((m_writeDependencies | other.m_writeDependencies) != 0)
    {
        return true;
    }

    if ((m_writeDependencies | other.m_readDependencies) != 0)
    {
        return true;
    }

    if ((m_readDependencies | other.m_writeDependencies) != 0)
    {
        return true;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobDependencies::IsLowerPriorityThan(JobDependencies const& other) const
{
    if (m_priority == -1 || other.m_priority == -1)
    {
        return false;
    }
    else return m_priority < other.m_priority;
}
