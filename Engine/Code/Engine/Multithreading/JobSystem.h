// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/EngineSubsystem.h"
#include "Engine/DataStructures/ThreadSafePrioQueue.h"
#include "Job.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>



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
    bool m_deditatedLoadingWorker = true;
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
    
    JobSystem(JobSystemConfig const& config = JobSystemConfig());
    virtual ~JobSystem() override;
    
    virtual void Startup() override;
    virtual void Shutdown() override;

    void CreateJobWorker(int threadID, std::string const& name = "");
    void CreateLoadingJobWorker(int threadID, std::string const& name = "");
    
    JobID PostJob(Job* job);
    JobID PostLoadingJob(Job* job);
    std::vector<JobID> PostJobs(std::vector<Job*>& jobs);
	bool TryCancelJob(JobID jobID);
	bool TryCancelLoadingJob(JobID jobID);
	int TryCancelLoadingJobs(std::vector<JobID> const& jobIDs);

    // Completing Jobs: returns true when all jobs in question are complete or don't exist
    bool CompleteJob(JobID jobID, bool blockAndHelp = true);
    bool CompleteJobs(std::vector<JobID>& in_out_ids, bool blockAndHelp = true);
    bool WaitForAllJobs(bool blockAndHelp = true);
    
    void ExecuteJobGraph(JobGraph& jobGraph, bool helpWithTasksOnThisThread = false);

    JobSystemConfig const m_config;

protected:

    // General purpose workers
    void WorkerLoop(JobWorker* worker);
    bool WorkerLoop_TryDoFirstAvailableJob(JobWorker* worker, bool blocking = true);
    Job* PopFirstAvailableJob(bool blocking = true);
    Job* PopFirstAvailableLoadingJob(bool blocking = true);
    void WorkerLoop_ExecuteJob(JobWorker* worker, Job* job);

    // Loading worker
    void LoadingWorkerLoop(JobWorker* worker);
    bool LoadingWorkerLoop_TryDoFirstAvailableJob(JobWorker* worker, bool blocking = true);

    // While waiting to complete jobs, threads can try to complete the jobs they are waiting on, which may recur
    bool TryDoSpecificJob(JobID jobToExpedite);
    bool TryDoSpecificJobs(std::vector<JobID> const& jobIDs);

    void PostAvailableJobGraphTasks(JobGraph& graph);
    void CompleteAvailableJobGraphTasks(JobGraph& graph);

    void AddJobToInProgressQueue(Job* job);
    void RemoveJobFromInProgressQueue(Job* job);
    
    void AddJobToCompletedQueue(Job* job);
    Job* RemoveJobFromCompletedQueue(JobID jobID);

protected:
    
    static uint32_t GetNextJobUniqueID();
    
protected:

    std::atomic<bool>           m_isRunning                 = true;
    std::atomic<int>            m_numIncompleteJobs         = 0;

    std::vector<JobWorker*>     m_workers;

    ThreadSafePrioQueue<Job>    m_jobQueue;
	ThreadSafePrioQueue<Job>    m_loadingJobQueue;          // Queue specifically for jobs that touch the disk, always a dedicated worker
    
    std::mutex                  m_inProgressJobsMutex;
    std::vector<Job*>           m_inProgressJobs;

    std::mutex                  m_completedJobsMutex;
    std::vector<Job*>           m_completedJobs;

    std::atomic<uint32_t>       m_nextJobID                 = 0;
};





