// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Job.h"
#include <vector>
#include <mutex>
#include <atomic>



struct JobWorker;
class Job;



extern class JobSystem* g_theJobSystem;



struct JobSystemConfig
{
    uint32_t m_threadCount = std::thread::hardware_concurrency();
};



//----------------------------------------------------------------------------------------------------------------------
// Job System
//
// Owns worker threads and runs jobs.
//
class JobSystem : public EngineSubsystem
{
    friend class Job;
    
public:
    
    JobSystem(JobSystemConfig const& config);
    virtual ~JobSystem() override = default;
    
    virtual void Startup() override;
    virtual void Shutdown() override;
    
    JobID const PostJob(Job* job);
    void WaitForJob(JobID jobID);
    void WaitForAllJobs();

    JobSystemConfig const m_config;

protected:

    void WorkerLoop(JobWorker* worker);
    Job* ClaimNextJob();

protected:
    
    static uint32_t GetNextJobUniqueID();
    
protected:

    // Master switch to halt all jobs
    std::atomic<bool>       m_isRunning = true;

    // Worker Threads
    std::vector<JobWorker*> m_workers;

    // Job System lock
    std::mutex              m_jobSystemMutex;

    // Job Statuses
    std::vector<JobStatus>  m_jobStatuses;

    // Queued Jobs
    std::vector<Job*>       m_jobQueue;
    int                     m_numJobsReadyToStart = 0;
    
    // Cond var that is notified when jobs are posted
    std::condition_variable m_jobPostedCondVar;

    // Cond var that is notified when jobs are complete
    std::condition_variable m_jobCompleteCondVar;

    // Task ID Tracker
    std::atomic<uint32_t>   m_nextJobID = 0;
};