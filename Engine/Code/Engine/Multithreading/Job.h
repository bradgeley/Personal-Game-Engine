// Bradley Christensen - 2022
#pragma once
#include "JobID.h"
#include <cstdint>



enum class JobStatus : uint8_t
{
    Null, 
    Posted,
    Claimed,
};



//----------------------------------------------------------------------------------------------------------------------
// Job
//
// A unit of work, executed on an arbitrary thread
//
class Job
{
public:
    
    virtual ~Job() = default;
    
    virtual void Execute() {}

    bool IsValid() const;
    uint32_t GetIndex() const;
    uint32_t GetUniqueID() const;

public:
    
    JobID m_id                   = JobID::Invalid;
    
    uint64_t m_frameNumber       = 0;
    uint64_t m_readDependencies  = 0;
    uint64_t m_writeDependencies = 0;
};