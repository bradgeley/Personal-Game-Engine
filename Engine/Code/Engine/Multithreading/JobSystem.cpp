// Bradley Christensen - 2022
#include "Engine/Multithreading/JobSystem.h"
#include "JobWorker.h"
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

    // Delete completed jobs that never got claimed
    for (auto& job : m_completedJobsQueue)
    {
        // job->Complete(); jobs should not get completed during shutdown, that's dangerous bc most game/engine state is probably gone by now
        delete job;
        job = nullptr;
    }
    
    // Shut down all workers
    for (int i = 0; i < (int) m_workers.size(); ++i)
    {
        JobWorker*& worker = m_workers[i];
        worker->Shutdown();
        delete worker;
        worker = nullptr;
    }
    
    m_completedJobsQueue.clear();
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
        m_jobStatuses.emplace_back(JobStatus::Posted);
        m_jobQueue.emplace_back(job);
    }
    else
    {
        m_jobStatuses[newJobIndex] = JobStatus::Posted;
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
void JobSystem::WaitForJob(JobID jobID)
{
    if (jobID == JobID::Invalid)
    {
        return;
    }

    std::unique_lock jobSystemLock(m_jobSystemMutex);
    while (true)
    {
        if (m_jobStatuses[jobID.m_index] == JobStatus::Null)
        {
            // The job was finished and the slot it was in is now open
            return;
        }
        Job*& job = m_jobQueue[jobID.m_index];
        if (!job)
        {
            // the job is done and deleted
            return;
        }
        if (job->m_id != jobID)
        {
            // The job was finished and replaced by another
            return;
        }
        
        m_jobCompleteCondVar.wait(jobSystemLock);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::WaitForAllJobs()
{
    while (m_isRunning)
    {
        bool isOneJobIncomplete = false;
        
        m_jobSystemMutex.lock();
        for (JobStatus& status : m_jobStatuses)
        {
            if (status != JobStatus::Null)
            {
                isOneJobIncomplete = true;
                break;
            }
        }
        m_jobSystemMutex.unlock();

        if (!isOneJobIncomplete)
        {
            // not one job is incomplete, so all jobs are complete
            return;
        }
        
        std::this_thread::yield();
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::CompleteJob(JobID id)
{
    std::unique_lock lock(m_completedJobsMutex);
    for (int i = 0; i < (int) m_completedJobsQueue.size(); ++i)
    {
        auto& job = m_completedJobsQueue[i];
        if (id == job->m_id)
        {
            job->Complete();
            m_completedJobsQueue.erase(m_completedJobsQueue.begin() + i);
            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::CompleteJobs(std::vector<JobID>& in_out_ids)
{
    int numCompleted = 0;
    std::unique_lock lock(m_completedJobsMutex);
    for (int i = (int) m_completedJobsQueue.size() - 1; i >= 0; --i)
    {
        Job*& job = m_completedJobsQueue[i];
        for (int j = (int) in_out_ids.size() - 1; j >= 0; --j)
        {
            JobID& id = in_out_ids[j];
            if (id == job->m_id)
            {
                job->Complete();
                ++numCompleted;

                delete job;
                job = nullptr;
                
                m_completedJobsQueue.erase(m_completedJobsQueue.begin() + i);
                in_out_ids.erase(in_out_ids.begin() + j);
                break;
            }
        }
    }
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
        Job* job = ClaimNextJob();
        if (job)
        {
            // Execute the job
            job->Execute();
            
            uint32_t index = job->GetIndex();

            bool needsComplete = job->NeedsComplete();
            if (needsComplete)
            {
                // Send the job over to the completed queue to be picked up by the main thread on the next update
                m_completedJobsMutex.lock();
                m_completedJobsQueue.emplace_back(job);
                m_completedJobsMutex.unlock();
            }
            else
            {
                // Just finish the job now
                delete m_jobQueue[index];
                m_jobQueue[index] = nullptr;
            }

            // Open slot in the job queue
            m_jobSystemMutex.lock();
            m_jobStatuses[index] = JobStatus::Null;
            m_jobSystemMutex.unlock();

            m_jobCompleteCondVar.notify_all();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
Job* JobSystem::ClaimNextJob()
{
    Job* result = nullptr;
    std::unique_lock uniqueLock(m_jobSystemMutex);
    while (m_isRunning && m_numJobsReadyToStart == 0)
    {
        m_jobPostedCondVar.wait(uniqueLock);
    }
    
    for (int i = 0; i < (int) m_jobStatuses.size(); ++i)
    {
        JobStatus& status = m_jobStatuses[i];
        if (status == JobStatus::Posted)
        {
            status = JobStatus::Claimed; // claimed
            m_numJobsReadyToStart--;
            result = m_jobQueue[i];
            break;
        }
    }
    return result;
}
