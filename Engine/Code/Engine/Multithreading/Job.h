// Bradley Christensen - 2022-2025
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
    friend class JobSystem;

public:
    
    virtual ~Job() = default;
    
    JobDependencies const& GetJobDependencies() const           { return m_jobDependencies; }
    bool NeedsComplete() const                                  { return m_needsComplete; }
    bool DeleteAfterCompletion() const                          { return m_deleteAfterCompletion; }
    int GetJobPriority() const                                  { return m_priority; }

	void SetNeedsComplete(bool needsComplete)                   { m_needsComplete = needsComplete; }
	void SetDeleteAfterCompletion(bool deleteAfterCompletion)   { m_deleteAfterCompletion = deleteAfterCompletion; }
	void SetPriority(int priority)                              { m_priority = priority; }

    bool IsValid() const;
    bool HasDependencies() const;
    uint32_t GetUniqueID() const;

    bool operator<(Job const& rhs) const
    {
        return m_priority < rhs.m_priority;
	}

protected:

    virtual void Execute()                              {}
    virtual void Complete()                             {}

protected:

    JobID               m_id                            = 0;
    JobDependencies     m_jobDependencies;
    bool                m_needsComplete                 = true;
    bool                m_deleteAfterCompletion         = true;
    int                 m_priority                      = -1;       // Lower is better
};