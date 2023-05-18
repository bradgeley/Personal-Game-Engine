// Bradley Christensen - 2023
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct JobDependencies
{
    bool SharesDependencies(JobDependencies const& other) const;
    bool IsLowerPriorityThan(JobDependencies const& other) const;

    int m_priority = -1;
    uint64_t m_readDependencies  = 0;
    uint64_t m_writeDependencies = 0;
};
