// Bradley Christensen - 2023
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct JobDependencies
{
public:

    JobDependencies() = default;
    JobDependencies(uint64_t const& readDeps, uint64_t const& writeDeps);
    bool SharesDependencies(JobDependencies const& other) const;

public:

    uint64_t m_readDependencies  = 0;
    uint64_t m_writeDependencies = 0;
};
