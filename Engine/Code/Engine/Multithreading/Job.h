// Bradley Christensen - 2022
#pragma once
#include "JobID.h"
#include "JobDependencies.h"
#include <cstdint>



//----------------------------------------------------------------------------------------------------------------------
enum class JobStatus : uint8_t
{
    // Used by job system to manage pending jobs:
    Null,           // Empty slot in the job queue
    Posted,         // Posted, waiting to be picked up by a thread
    Running,        // Claimed by a thread, and is executing.

    // Used by job graph only - in the job system executed tasks go straight to the completed jobs queue and open up a slot:
    Completed,      // Complete() and/or Execute() has been called (depending on if the task requires complete)
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
    virtual JobDependencies GetJobDependencies() const { return {}; }
    virtual int GetJobPriority() const { return -1; }

    bool IsValid() const;
    bool HasDependencies() const;
    uint32_t GetUniqueID() const;

protected:
    
    friend class JobSystem;

    // Called from the job system when the main thread asks to complete its JobID
    virtual void Complete() {}

protected:

    JobID m_id = 0;
};