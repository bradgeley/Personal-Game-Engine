// Bradley Christensen - 2022
#pragma once
#include <vector>
#include "Job.h"
#include "JobID.h"



//----------------------------------------------------------------------------------------------------------------------
// Job Graph
//
// A graph of jobs where each job will complete as it becomes available for it to do so
//
struct JobGraph
{
    friend class JobSystem;

public:

    void AddJob(Job* job);
    bool IsComplete() const;
    void Cleanup();
    void Reserve(size_t numExpectedJobs);
    
protected:
    
    void Reset();
    void SortByPriority();
    
protected:
    
    std::vector<Job*> m_jobs;

    // Transient data
    std::vector<JobID> m_jobReceipts;
    std::vector<JobStatus> m_jobStatuses;
    std::vector<JobDependencies> m_jobDeps;
};