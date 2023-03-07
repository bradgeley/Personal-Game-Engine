// Bradley Christensen - 2022
#pragma once
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
struct JobID
{
    friend class Job;
    friend class JobSystem;

    JobID(uint32_t index, uint32_t uniqueID);

    bool operator==(JobID const& rhs) const;
    bool operator!=(JobID const& rhs) const;
    
    uint32_t m_index    = 0; // index into the job queue
    uint32_t m_uniqueID = 0; // unique ID

public:

    static JobID Invalid;
};