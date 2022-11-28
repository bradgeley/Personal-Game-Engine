// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/DataStructures/ThreadSafeQueue.h"
#include "Job.h"
#include <thread>
#include <vector>
#include <atomic>



struct JobWorker;



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
    friend struct Job;
    
public:
    
    JobSystem(JobSystemConfig const& config);
    
    void QueueJob(Job* job);
    void WaitForAllJobs();
    
    virtual void Startup() override;
    virtual void Shutdown() override;

    JobSystemConfig const m_config;

protected:

    static JobID GetNextJobID();
    void WorkerLoop(JobWorker* worker);
    Job* GetNextJob();

protected:
    
    std::atomic<bool>       m_isRunning = true;

    // Worker Threads
    std::vector<JobWorker*> m_workers;

    // Queued Jobs
    std::mutex              m_jobQueueMutex;
    std::condition_variable m_jobCondVar;
    std::vector<Job*>       m_jobQueue;

    // Task ID Tracker
    std::atomic<int>        m_nextJobID = 0;
};