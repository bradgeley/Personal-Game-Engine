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
    
    bool NeedsComplete() const { return m_needsComplete; }
    bool DeleteAfterCompletion() const { return m_deleteAfterCompletion; }
    JobDependencies const& GetJobDependencies() const { return m_jobDependencies; }
    int GetJobPriority() const { return m_priority; }

    bool IsValid() const;
    bool HasDependencies() const;
    uint32_t GetUniqueID() const;

protected:
    
    friend class JobSystem;

    virtual void Execute() {}
    virtual void Complete() {}

public:

    bool m_needsComplete = true;
    bool m_deleteAfterCompletion = true;
    JobDependencies m_jobDependencies;
    int m_priority = -1;

protected:

    JobID m_id = 0;
};