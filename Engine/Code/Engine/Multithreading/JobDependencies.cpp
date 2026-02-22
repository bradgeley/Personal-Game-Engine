// Bradley Christensen - 2022-2026
#include "JobDependencies.h"



//----------------------------------------------------------------------------------------------------------------------
JobDependencies::JobDependencies(uint64_t const& readDeps, uint64_t const& writeDeps) : m_readDependencies(readDeps), m_writeDependencies(writeDeps)
{
}



//----------------------------------------------------------------------------------------------------------------------
bool JobDependencies::SharesDependencies(JobDependencies const& other) const
{
    if ((m_writeDependencies & other.m_writeDependencies) != 0)
    {
        return true;
    }

    if ((m_writeDependencies & other.m_readDependencies) != 0)
    {
        return true;
    }

    if ((m_readDependencies & other.m_writeDependencies) != 0)
    {
        return true;
    }

    return false;
}
