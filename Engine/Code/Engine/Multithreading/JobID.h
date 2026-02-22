// Bradley Christensen - 2022-2026
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct JobID
{
    friend class Job;
    friend class JobSystem;

public:

    JobID();
    JobID(uint32_t uniqueID);

    bool operator<(JobID const& rhs) const;
    bool operator==(JobID const& rhs) const;
    bool operator!=(JobID const& rhs) const;
    
    uint32_t m_uniqueID = 0;

public:

    static JobID Invalid;
};