// Bradley Christensen - 2022
#include "Engine/Multithreading/JobSystem.h"
#include "JobDependencies.h"
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

    m_jobDependencies.reserve(100);
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
    
    m_jobSystemMutex.lock();

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
        m_jobDependencies.emplace_back(job->m_dependencies);
        m_jobQueue.emplace_back(job);
    }
    else
    {
        // Fill the empty space
        m_jobStatuses[newJobIndex] = JobStatus::Posted;
        m_jobDependencies[newJobIndex] = job->m_dependencies;
        m_jobQueue[newJobIndex] = job;
    }

    // Set the index of the job to the slot we just added it to
    job->m_id.m_index = (uint32_t) newJobIndex;
    JobID result = job->m_id;
    
    // Increase count so waiting threads know to check for jobs
    m_numJobsReadyToStart++;
    
    m_jobSystemMutex.unlock();

    // Notify waiting threads
    m_jobPostedCondVar.notify_one();
    return result;
}



//----------------------------------------------------------------------------------------------------------------------
std::vector<JobID> JobSystem::PostJobs(std::vector<Job*>& jobs, bool autoPrioritize)
{
    if (autoPrioritize)
    {
        int priority = 0;
        for (auto& job : jobs)
        {
            job->m_dependencies.m_priority = priority;
            priority++;
        }    
    }
    
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
    std::unique_lock jobSystemLock(m_jobSystemMutex);
    while (true)
    {
        if (!IsJobValid_Locked(jobID))
        {
            return;
        }

        JobStatus status = m_jobStatuses[jobID.m_index];
        if (status == JobStatus::Null || status == JobStatus::Executed)
        {
            return;
        }

        
        
        m_jobExecutedCondVar.wait(jobSystemLock);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::WaitForAllJobs()
{
    while (m_isRunning)
    {
        std::unique_lock lock(m_jobSystemMutex);

        bool allJobsExecuted = true;
        for (JobStatus& status : m_jobStatuses)
        {
            if (status == JobStatus::Posted || status == JobStatus::Running)
            {
                allJobsExecuted = false;
                break;
            }
        }

        if (allJobsExecuted)
        {
            return;
        }

        m_jobExecutedCondVar.wait(lock);
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::CompleteJob(JobID id)
{
    std::unique_lock lock(m_jobSystemMutex);

    if (IsJobValid_Locked(id))
    {
        uint32_t& index = id.m_index;
        auto& status = m_jobStatuses[index];
        if (status != JobStatus::Executed)
        {
            return false;
        }

        // Job has been executed... proceed
        auto& job = m_jobQueue[index];
        
        job->Complete();

        DeleteJob_Locked(id);
        
        return true;
    }
    
    return false;
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
bool JobSystem::IsJobValid(JobID id)
{
    std::unique_lock lock(m_jobSystemMutex);
    return IsJobValid_Locked(id);
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
        job->Execute();
            
        uint32_t jobIndex = job->GetIndex();
        bool needsComplete = job->NeedsComplete();
        if (needsComplete)
        {
            m_jobSystemMutex.lock();
            m_jobStatuses[jobIndex] = JobStatus::Executed;
            m_jobSystemMutex.unlock();
        }
        else
        {
            m_jobSystemMutex.lock();
            DeleteJob_Locked(job->m_id);
            m_jobSystemMutex.unlock();
        }

        m_jobExecutedCondVar.notify_all();
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
Job* JobSystem::ClaimNextJob()
{
    std::unique_lock uniqueLock(m_jobSystemMutex);
    
    while (m_isRunning && m_numJobsReadyToStart == 0)
    {
        m_jobPostedCondVar.wait(uniqueLock);
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
bool JobSystem::IsJobValid_Locked(JobID id) const
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

    Job* const& job = m_jobQueue[index];
    if (job->m_id != id)
    {
        return false;
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
    
    Job* const& job = m_jobQueue[index];

    // Check other jobs in the queue to determine if this job can run
    for (int jobIndex = 0; jobIndex < (int) m_jobQueue.size(); ++jobIndex)
    {
        if (jobIndex == index)
        {
            // Don't compare to self
            continue;
        }
        
        auto& status = m_jobStatuses[jobIndex];
        if (status == JobStatus::Null || status == JobStatus::Executed)
        {
            continue;
        }
        
        auto& deps = m_jobDependencies[jobIndex];
        bool bothJobsShareDependencies = deps.SharesDependencies(job->m_dependencies);
        if (status == JobStatus::Running && bothJobsShareDependencies)
        {
            // A job is running which shares dependencies with this job - can't run
            return false;
        }
        if (status == JobStatus::Posted && deps.IsLowerPriorityThan(job->m_dependencies) && bothJobsShareDependencies)
        {
            // Another job is posted that is lower priority than this job and shares dependencies, it needs to start first - can't run 
            return false;
        }
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