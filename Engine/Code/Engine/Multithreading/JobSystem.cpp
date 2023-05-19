// Bradley Christensen - 2022
#include "Engine/Multithreading/JobSystem.h"
#include "JobDependencies.h"
#include "JobGraph.h"
#include "JobWorker.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
// THE JOB SYSTEM
//
JobSystem* g_jobSystem = nullptr;



//----------------------------------------------------------------------------------------------------------------------
JobSystem::JobSystem(JobSystemConfig const& config) : m_config(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::Startup()
{
    EngineSubsystem::Startup();

    m_jobStatuses.reserve(100);
    m_jobQueue.reserve(100);

    // initialize worker threads
    for (int i = 0; i < (int) m_config.m_threadCount; ++i)
    {
        JobWorker* worker = new JobWorker();
        worker->m_thread = std::thread(&JobSystem::WorkerLoop, this, worker );
        worker->m_name = StringF("JobSystemWorker: %i", i);
        m_workers.emplace_back( worker );
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::Shutdown()
{
    EngineSubsystem::Shutdown();

    // Finish the job queue, all jobs are now nullptr so no need to do any additional cleanup except the completed queue
    WaitForAllJobs();

    // Wake up all idle threads and tell them we aren't running anymore
    m_isRunning = false;
    m_jobPostedCondVar.notify_all();
    
    // Shut down all workers
    for (auto& worker : m_workers)
    {
        worker->Shutdown();
        delete worker;
        worker = nullptr;
    }
    
    m_workers.clear();
    m_jobQueue.clear();
    m_jobStatuses.clear();
}



//----------------------------------------------------------------------------------------------------------------------
JobID JobSystem::PostJob(Job* job)
{
    if (!job || !m_isRunning)
    {
        return JobID::Invalid;
    }

    job->m_id.m_uniqueID = GetNextJobUniqueID();

    std::unique_lock lock(m_jobSystemMutex);

    // Find an empty index to put the new job in
    int newJobIndex;
    for (newJobIndex = 0; newJobIndex < (int) m_jobStatuses.size(); ++newJobIndex)
    {
        if (m_jobStatuses[newJobIndex] == JobStatus::Null)
        {
            break;
        }
    }
    
    if (newJobIndex == (int) m_jobStatuses.size())
    {
        // No empty space, grow the vector
        m_jobStatuses.emplace_back(JobStatus::Posted);
        m_jobQueue.emplace_back(job);
    }
    else
    {
        // Fill the empty space
        m_jobStatuses[newJobIndex] = JobStatus::Posted;
        m_jobQueue[newJobIndex] = job;
    }

    // Set the index of the job to the slot we just added it to
    job->m_id.m_index = (uint32_t) newJobIndex;
    JobID result = job->m_id;
    
    // Increase count so waiting threads know to check for jobs
    m_numJobsReadyToStart++;
    
    // Notify waiting threads
    m_jobPostedCondVar.notify_one();

    return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<JobID> JobSystem::PostJobs(std::vector<Job*>& jobs)
{
    std::vector<JobID> result;
    result.resize(jobs.size());
    for (auto& job : jobs)
    {
       result.emplace_back(PostJob(job));
    }
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::WaitForJob(JobID jobID)
{
    while (true)
    {
        if (!IsJobExecuting(jobID))
        {
            return;
        }

        std::unique_lock lock(m_jobSystemMutex);
        JobStatus const& status = m_jobStatuses[jobID.m_index];
        if (status == JobStatus::Null)
        {
            return;
        }
        lock.unlock();
        
        if (!TryDoOneJob_NonBlocking())
        {
            std::this_thread::yield();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::WaitForAllJobs()
{
    while (m_isRunning)
    {
        bool allJobsExecuted = true;

        std::unique_lock lock(m_jobSystemMutex);
        for (JobStatus& status : m_jobStatuses)
        {
            if (status == JobStatus::Posted || status == JobStatus::Running)
            {
                allJobsExecuted = false;
                break;
            }
        }
        lock.unlock();

        if (allJobsExecuted)
        {
            return;
        }

        if (!TryDoOneJob_NonBlocking())
        {
            std::this_thread::yield();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::ExecuteJobGraph(JobGraph& jobGraph, bool helpWithTasksOnThisThread)
{
    jobGraph.Initialize();
    
    while (!jobGraph.IsComplete())
    {
        PostAvailableJobGraphTasks(jobGraph);

        if (helpWithTasksOnThisThread)
        {
            TryDoOneJob_NonBlocking(); // todo: may only do jobs in the jobGraph we just posted
        }
        else std::this_thread::yield();
        
        CompleteAvailableJobGraphTasks(jobGraph);
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::CompleteJob(JobID id)
{
    std::unique_lock lock(m_completedJobsMutex);
    return CompleteJob_Locked(id);
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::CompleteJobs(std::vector<JobID>& in_out_ids)
{
    for (auto& id : in_out_ids)
    {
        CompleteJob(id);
    }
}
 


//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::IsJobExecuting(JobID id)
{
    std::unique_lock lock(m_jobSystemMutex);
    return IsJobExecuting_Locked(id);
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t JobSystem::GetNextJobUniqueID()
{
    return g_jobSystem->m_nextJobID.fetch_add(1);
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::WorkerLoop(JobWorker* worker)
{
    while (m_isRunning && worker->m_isRunning)
    {
        //if (!TryDoOneJob_NonBlocking())
        if (!WorkerLoop_TryDoOneJob())
        {
            std::this_thread::yield();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::WorkerLoop_TryDoOneJob()
{
    Job* job = ClaimNextJob();
    if (job)
    {
        WorkerLoop_ExecuteJob(job);
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::WorkerLoop_ExecuteJob(Job* job)
{
    job->Execute();
            
    uint32_t index = job->GetIndex();
        
    if (job->NeedsComplete())
    {
        std::unique_lock lock(m_completedJobsMutex);
        m_completedJobsQueue.emplace_back(job);
    }
    else
    {
        delete m_jobQueue[index];
        m_jobQueue[index] = nullptr;
    }
    
    std::unique_lock lock(m_jobSystemMutex);
    m_jobStatuses[index] = JobStatus::Null;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::TryDoOneJob_NonBlocking()
{
    Job* job = ClaimNextJob_NonBlocking();
    if (job)
    {
        WorkerLoop_ExecuteJob(job);
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
Job* JobSystem::ClaimNextJob()
{
    std::unique_lock lock(m_jobSystemMutex);
    
    while (m_isRunning && m_numJobsReadyToStart == 0)
    {
        m_jobPostedCondVar.wait(lock);
    }

    if (m_numJobsReadyToStart > 0)
    {
        for (int jobIndex = 0; jobIndex < (int) m_jobStatuses.size(); ++jobIndex)
        {
            if (CanJobAtIndexRun_Locked(jobIndex))
            {
                m_jobStatuses[jobIndex] = JobStatus::Running; // claimed
                m_numJobsReadyToStart--;
                return m_jobQueue[jobIndex];
            }
        }
    }
    
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
Job* JobSystem::ClaimNextJob_NonBlocking()
{
    std::unique_lock lock(m_jobSystemMutex);
    
    if (m_isRunning && m_numJobsReadyToStart == 0)
    {
        return nullptr;
    }

    for (int jobIndex = 0; jobIndex < (int) m_jobStatuses.size(); ++jobIndex)
    {
        if (CanJobAtIndexRun_Locked(jobIndex))
        {
            m_jobStatuses[jobIndex] = JobStatus::Running; // claimed
            m_numJobsReadyToStart--;
            return m_jobQueue[jobIndex];
        }
    }

    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::PostAvailableJobGraphTasks(JobGraph& graph)
{
    for (int jobIndex = 0; jobIndex < (int) graph.m_jobs.size(); ++jobIndex)
    {
        Job*& job = graph.m_jobs[jobIndex];
        auto& status = graph.m_jobStatuses[jobIndex];
        auto& deps = graph.m_jobDeps[jobIndex];
        if (status == JobStatus::Posted || status == JobStatus::Completed)
        {
            continue;
        }

        bool canRun = true;
        for (int jobBeforeIndex = 0; jobBeforeIndex < jobIndex; ++jobBeforeIndex)
        {
            if (graph.m_jobStatuses[jobBeforeIndex] != JobStatus::Completed)
            {
                JobDependencies& jobBeforeDeps = graph.m_jobDeps[jobBeforeIndex];
                if (deps.SharesDependencies(jobBeforeDeps))
                {
                    canRun = false;
                    break;
                }
            }
        }

        if (canRun)
        {
            graph.m_jobReceipts[jobIndex] = PostJob(job);
            graph.m_jobStatuses[jobIndex] = JobStatus::Posted;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::CompleteAvailableJobGraphTasks(JobGraph& graph)
{
    for (int jobIndex = 0; jobIndex < (int) graph.m_jobs.size(); ++jobIndex)
    {
        JobID id = graph.m_jobReceipts[jobIndex];
        JobStatus& status = graph.m_jobStatuses[jobIndex];
        
        if (status == JobStatus::Posted)
        {
            if (!IsJobExecuting(id))
            {
                std::unique_lock lock(m_completedJobsMutex);
                if (IsJobWaitingForComplete_Locked(id))
                {
                    CompleteJob_Locked(id);
                }
                status = JobStatus::Completed;
            }
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
// Executing means not Null (either posted or running), with a valid index and Job* in the job queue
//
bool JobSystem::IsJobExecuting_Locked(JobID id) const
{
    uint32_t const& index = id.m_index;
    if (index < 0 || index >= m_jobQueue.size())
    {
        return false;
    }

    JobStatus const& status = m_jobStatuses[index];
    if (status == JobStatus::Null)
    {
        return false;
    }

    if (status == JobStatus::Running || status == JobStatus::Posted)
    {
        return true;
    }

    return true;
}



//----------------------------------------------------------------------------------------------------------------------
// Multithreading warning: Only call if the m_jobSystemMutex is locked.
//
bool JobSystem::CanJobAtIndexRun_Locked(int index) const
{
    if (index < 0 || index >= (int) m_jobStatuses.size())
    {
        // index out of bounds - error msg?
        return false;
    }
    
    JobStatus const& jobStatus = m_jobStatuses[index];
    if (jobStatus != JobStatus::Posted)
    {
        // either null or already claimed
        return false;
    }
    
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::DeleteJob_Locked(JobID id)
{
    delete m_jobQueue[id.m_index];
    m_jobQueue[id.m_index] = nullptr;
    m_jobStatuses[id.m_index] = JobStatus::Null;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::CompleteJob_Locked(JobID id)
{
    for (int completedJobIndex = 0; completedJobIndex < (int) m_completedJobsQueue.size(); ++completedJobIndex)
    {
        Job*& job = m_completedJobsQueue[completedJobIndex];
        if (job->m_id == id)
        {
            job->Complete();

            delete job;
            job = nullptr;
            
            m_completedJobsQueue.erase(m_completedJobsQueue.begin() + completedJobIndex);
            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::IsJobWaitingForComplete_Locked(JobID id) const
{
    for (auto& job : m_completedJobsQueue)
    {
        if (job->m_id == id)
        {
            return true;
        }
    }
    return false;
}