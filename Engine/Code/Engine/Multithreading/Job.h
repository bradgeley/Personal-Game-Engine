// Bradley Christensen - 2022
#pragma once
#include "JobID.h"
#include "JobDependencies.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class JobStatus : uint8_t
{
    Null, 
    Posted,
    Running,
    Executed,
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
    bool HasDependencies() const;
    uint32_t GetIndex() const;
    uint32_t GetUniqueID() const;

protected:
    friend class JobSystem;

    // Called from the job system when the main thread asks to complete its JobID
    virtual void Complete() {}

protected:
    
    JobID m_id                   = JobID::Invalid;

public:
    
    JobDependencies m_dependencies;
};