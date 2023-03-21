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
    virtual bool NeedsComplete() { return false; }

    bool IsValid() const;
    uint32_t GetIndex() const;
    uint32_t GetUniqueID() const;

protected:
    friend class JobSystem;

    // Called from the job system when the main thread asks to complete its JobID
    virtual void Complete() {}

public:
    
    JobID m_id                   = JobID::Invalid;
    
    uint64_t m_frameNumber       = 0;
    uint64_t m_readDependencies  = 0;
    uint64_t m_writeDependencies = 0;
};