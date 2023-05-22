﻿// Bradley Christensen - 2022
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
    
protected:
    
    void Initialize();
    void SortByPriority();
    
protected:
    
    std::vector<Job*> m_jobs;
    std::vector<JobID> m_jobReceipts;
    std::vector<JobStatus> m_jobStatuses;
    std::vector<JobDependencies> m_jobDeps;
};