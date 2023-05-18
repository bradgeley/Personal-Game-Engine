// Bradley Christensen - 2022
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "JobDependencies.h"
#include "Job.h"
#include <vector>
#include <mutex>
#include <atomic>
#include <deque>



struct JobWorker;
struct JobGraph;
class Job;



//----------------------------------------------------------------------------------------------------------------------
// THE Job System
//
extern class JobSystem* g_jobSystem;



//----------------------------------------------------------------------------------------------------------------------
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
    
    JobID PostJob(Job* job);
    std::vector<JobID> PostJobs(std::vector<Job*>& jobs, bool autoPrioritize = false);
    void WaitForJob(JobID jobID);
    void WaitForAllJobs();

    // Call from the main thread to complete jobs that require it
    bool CompleteJob(JobID id);
    void CompleteJobs(std::vector<JobID>& in_out_ids);
    
    bool IsJobValid(JobID id);

    JobSystemConfig const m_config;

protected:

    void WorkerLoop(JobWorker* worker);
    bool WorkerLoop_TryDoOneJob();
    Job* ClaimNextJob();
    
    // Must have the job system mutex locked to call
    bool IsJobValid_Locked(JobID id) const;
    bool CanJobAtIndexRun_Locked(int index) const;
    void DeleteJob_Locked(JobID id);

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

    // Job Dependencies - keeps track of currently running tasks' dependencies, so no tasks with clashing dep's get run at the same time.
    std::vector<JobDependencies> m_jobDependencies;

    // Queued Jobs
    std::vector<Job*>       m_jobQueue;
    int                     m_numJobsReadyToStart = 0;

    // Complete Jobs
    //std::mutex              m_completedJobsMutex;
    //std::vector<Job*>       m_completedJobsQueue;
    
    // Cond var that is notified when jobs are posted
    std::condition_variable m_jobPostedCondVar;

    // Cond var that is notified when jobs are complete - for 
    std::condition_variable m_jobExecutedCondVar;

    // Unique Job ID Tracker
    std::atomic<uint32_t>   m_nextJobID = 0;
};



