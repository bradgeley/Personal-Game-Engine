// Bradley Christensen - 2022-2025
#include "Engine/Multithreading/JobSystem.h"
#include "JobDependencies.h"
#include "JobGraph.h"
#include "JobWorker.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Time/Time.h"



//----------------------------------------------------------------------------------------------------------------------
// THE JOB SYSTEM
//
JobSystem* g_jobSystem = nullptr;



//----------------------------------------------------------------------------------------------------------------------
JobSystem::JobSystem(JobSystemConfig const& config) : EngineSubsystem("JobSystem"), m_config(config)
{

}



//----------------------------------------------------------------------------------------------------------------------
JobSystem::~JobSystem()
{
	g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::Startup()
{
    EngineSubsystem::Startup();

	int numGeneralThreads = (int) m_config.m_threadCount - m_config.m_deditatedLoadingWorker ? 1 : 0;
    for (int id = 0; id < numGeneralThreads; ++id)
    {
        CreateJobWorker(id);
    }

	CreateLoadingJobWorker(numGeneralThreads, "Loading Job Worker");
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::Shutdown()
{
    EngineSubsystem::Shutdown();

    // Finish the job queue, all jobs are now nullptr so no need to do any additional cleanup except the completed queue
    WaitForAllJobs(true);

    // Wake up all idle threads and tell them we aren't running anymore
    m_isRunning = false;
    
    m_jobQueue.Quit();
    m_loadingJobQueue.Quit();
    
    // Shut down all workers
    for (auto& worker : m_workers)
    {
        worker->Shutdown();
        delete worker;
        worker = nullptr;
    }
    
    m_workers.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::CreateJobWorker(int threadID, std::string const& name)
{
    JobWorker* worker = new JobWorker();
    worker->m_threadID = threadID;
    worker->m_thread = std::thread(&JobSystem::WorkerLoop, this, worker);
    worker->m_name = (!name.empty()) ? name : StringUtils::StringF("JobSystemWorker: %i", threadID);
    m_workers.emplace_back(worker);
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::CreateLoadingJobWorker(int threadID, std::string const& name)
{
    JobWorker* worker = new JobWorker();
    worker->m_threadID = threadID;
    worker->m_thread = std::thread(&JobSystem::LoadingWorkerLoop, this, worker);
    worker->m_name = (!name.empty()) ? name : StringUtils::StringF("JobSystemWorker: %i", threadID);
    m_workers.emplace_back(worker);
}



//----------------------------------------------------------------------------------------------------------------------
JobID JobSystem::PostJob(Job* job)
{
    if (!job || !m_isRunning)
    {
        return JobID::Invalid;
    }

    JobID id = GetNextJobUniqueID();
    job->m_id = id;
    
    ++m_numIncompleteJobs;
    m_jobQueue.Push(job);

    return id;
}



//----------------------------------------------------------------------------------------------------------------------
JobID JobSystem::PostLoadingJob(Job* job)
{
    if (!m_config.m_deditatedLoadingWorker)
    {
        return PostJob(job);
	}

    if (!job || !m_isRunning)
    {
        return JobID::Invalid;
    }

    JobID id = GetNextJobUniqueID();
    job->m_id = id;

    ++m_numIncompleteJobs;
    m_loadingJobQueue.Push(job);

    return id;
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
bool JobSystem::TryCancelJob(JobID jobID)
{
    m_jobQueue.Lock();
    for (auto it = m_jobQueue.begin(); it != m_jobQueue.end(); ++it)
    {
        if ((*it)->m_id != jobID)
        {
            continue;
		}
        m_numIncompleteJobs--;
        m_jobQueue.erase(it);
        m_jobQueue.Unlock();
        return true;
    }
    m_jobQueue.Unlock();
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::TryCancelLoadingJob(JobID jobID)
{
    m_loadingJobQueue.Lock();
    for (auto it = m_loadingJobQueue.begin(); it != m_loadingJobQueue.end(); ++it)
    {
        if ((*it)->m_id != jobID)
        {
            continue;
        }
        m_numIncompleteJobs--;
        m_loadingJobQueue.erase(it);
        m_loadingJobQueue.Unlock();
        return true;
    }
    m_loadingJobQueue.Unlock();
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
int JobSystem::TryCancelLoadingJobs(std::vector<JobID> const& jobIDs)
{
    int numCancelled = 0;
    m_loadingJobQueue.Lock();
    for (auto it = m_loadingJobQueue.begin(); it != m_loadingJobQueue.end();)
    {
        bool cancelled = false;

        for (JobID const& jobID : jobIDs)
        {
            if ((*it)->m_id != jobID)
            {
                continue;
            }

            cancelled = true;
            numCancelled++;
            m_numIncompleteJobs--;
            it = m_loadingJobQueue.erase(it);
            break;
        }

        if (!cancelled)
        {
            it++;
        }
    }
    m_loadingJobQueue.Unlock();
    return numCancelled;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::CompleteJob(JobID jobID, bool blockAndHelp /*= true*/)
{
    do 
    {
        Job* job = RemoveJobFromCompletedQueue(jobID);
        if (job)
        {
            bool completedJob = job->Complete();
            if (!completedJob)
            {
                // Allow us to retry later
                AddJobToCompletedQueue(job);
                continue;
            }

            --m_numIncompleteJobs;

            if (job->GetDeleteAfterCompletion())
            {
                delete job;
            }
            
            return true;
        }
        
        if (blockAndHelp)
        {
            if (!TryDoSpecificJob(jobID))
            {
                std::this_thread::yield();
            }
        }
    }
    while (blockAndHelp);
        
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::CompleteJobs(std::vector<JobID>& in_out_ids, bool blockAndHelp /*= true*/)
{
    do
    {
        for (auto it = in_out_ids.begin(); it != in_out_ids.end();)
        {
            JobID& jobID = *it;
            if (CompleteJob(jobID, false)) // non blocking here so we can complete in any order instead of blocking for each one in order
            {
                it = in_out_ids.erase(it);
            }
            else
            {
                ++it;
            }
        }
        
        if (blockAndHelp && !in_out_ids.empty())
        {
            if (!TryDoSpecificJobs(in_out_ids))
            {
                std::this_thread::yield();
            }
        }
    }
    while (blockAndHelp && !in_out_ids.empty());

    return in_out_ids.empty();
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::WaitForAllJobs(bool blockAndHelp)
{
    do
    {
        if (blockAndHelp && m_numIncompleteJobs > 0)
        {
            if (!WorkerLoop_TryDoFirstAvailableJob(nullptr, false))
            {
                std::this_thread::yield();
            }
        }

        std::vector<JobID> jobIDs;
        {
            std::unique_lock lock(m_completedJobsMutex);
            for (auto& jobs : m_completedJobs)
            {
                jobIDs.push_back(jobs->GetUniqueID());
            }
        }
        CompleteJobs(jobIDs);
    }
    while (m_numIncompleteJobs > 0);

    return m_numIncompleteJobs == 0;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::ExecuteJobGraph(JobGraph& jobGraph, bool helpWithTasksOnThisThread)
{
    jobGraph.Reset();
    
    while (!jobGraph.IsComplete())
    {
        PostAvailableJobGraphTasks(jobGraph);

        if (helpWithTasksOnThisThread)
        {
            TryDoSpecificJobs(jobGraph.m_jobReceipts);
        }
        else std::this_thread::yield();
        
        CompleteAvailableJobGraphTasks(jobGraph);
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
        if (!WorkerLoop_TryDoFirstAvailableJob(worker))
        {
            std::this_thread::yield();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::WorkerLoop_TryDoFirstAvailableJob(JobWorker* worker, bool blocking)
{
    Job* job = PopFirstAvailableJob(blocking);
    if (job)
    {
        WorkerLoop_ExecuteJob(worker, job);
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
Job* JobSystem::PopFirstAvailableJob(bool blocking)
{
    Job* job = m_jobQueue.Pop(blocking);
    return job;
}



//----------------------------------------------------------------------------------------------------------------------
Job* JobSystem::PopFirstAvailableLoadingJob(bool blocking)
{
    Job* job = m_loadingJobQueue.Pop(blocking);
    return job;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::WorkerLoop_ExecuteJob(JobWorker* worker, Job* job)
{
    UNUSED(worker)

    #ifdef _DEBUG
        AddJobToInProgressQueue(job);
    #endif

    job->Execute();

    #ifdef _DEBUG
        RemoveJobFromInProgressQueue(job);
    #endif

    if (job->GetNeedsComplete())
    {
        AddJobToCompletedQueue(job);
    }
    else
    {
        if (job->GetDeleteAfterCompletion())
        {
            delete job;
        }
        --m_numIncompleteJobs;
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::LoadingWorkerLoop(JobWorker* worker)
{
    while (m_isRunning && worker->m_isRunning)
    {
        if (!LoadingWorkerLoop_TryDoFirstAvailableJob(worker))
        {
            std::this_thread::yield();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::LoadingWorkerLoop_TryDoFirstAvailableJob(JobWorker* worker, bool blocking)
{
    Job* job = PopFirstAvailableLoadingJob(blocking);
    if (job)
    {
        WorkerLoop_ExecuteJob(worker, job);
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::TryDoSpecificJob(JobID jobToExpedite)
{
    Job* result = nullptr;
    
    m_jobQueue.Lock();
    for (auto it = m_jobQueue.begin(); it != m_jobQueue.end(); ++it)
    {
        Job* job = *it;
        if (job->m_id == jobToExpedite)
        {
            result = job;
            m_jobQueue.erase(it);
            break;
        }
    }
    m_jobQueue.Unlock();

    if (result)
    {
        WorkerLoop_ExecuteJob(nullptr, result);
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool JobSystem::TryDoSpecificJobs(std::vector<JobID> const& jobIDs)
{
    Job* result = nullptr;
    
    m_jobQueue.Lock();
    for (auto it = m_jobQueue.begin(); it != m_jobQueue.end(); ++it)
    {
        Job* job = *it;
        for (int i = 0; i < (int) jobIDs.size(); ++i)
        {
            if (jobIDs[i] == JobID::Invalid)
            {
                continue;
            }
            
            if (job->m_id == jobIDs[i])
            {
                result = job;
                m_jobQueue.erase(it);
                break;
            }
        }
        if (result)
        {
            break;
        }
    }
    m_jobQueue.Unlock();

    if (result)
    {
        WorkerLoop_ExecuteJob(nullptr, result);
        return true;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::PostAvailableJobGraphTasks(JobGraph& graph)
{
    for (int jobIndex = 0; jobIndex < (int) graph.m_jobs.size(); ++jobIndex)
    {
        auto& status = graph.m_jobStatuses[jobIndex];
        if (status == JobStatus::Posted || status == JobStatus::Completed)
        {
            continue;
        }

        JobDependencies& jobDeps = graph.m_jobDeps[jobIndex];

        bool canRun = true;
        for (int jobBeforeIndex = 0; jobBeforeIndex < jobIndex; ++jobBeforeIndex)
        {
            if (graph.m_jobStatuses[jobBeforeIndex] == JobStatus::Completed)
            {
                continue;
            }
            
            JobDependencies& jobBeforeDeps = graph.m_jobDeps[jobBeforeIndex];
            if (jobDeps.SharesDependencies(jobBeforeDeps))
            {
                canRun = false;
                break;
            }
        }

        if (canRun)
        {
            Job*& job = graph.m_jobs[jobIndex];
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
        JobStatus& status = graph.m_jobStatuses[jobIndex];
        if (status == JobStatus::Completed || status == JobStatus::Null)
        {
            continue;
        }

        JobID id = graph.m_jobReceipts[jobIndex];
        if (CompleteJob(id, false))
        {
            status = JobStatus::Completed;
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::AddJobToInProgressQueue(Job* job)
{
    m_inProgressJobsMutex.lock();
    m_inProgressJobs.emplace_back(job);
    m_inProgressJobsMutex.unlock();
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::RemoveJobFromInProgressQueue(Job* job)
{
    m_inProgressJobsMutex.lock();
    for (int i = 0; i < (int) m_inProgressJobs.size(); ++i)
    {
        if (m_inProgressJobs[i] == job)
        {
            m_inProgressJobs.erase(m_inProgressJobs.begin() + i);
            break;
        }
    }
    m_inProgressJobsMutex.unlock();
}



//----------------------------------------------------------------------------------------------------------------------
void JobSystem::AddJobToCompletedQueue(Job* job)
{
    m_completedJobsMutex.lock();
    m_completedJobs.emplace_back(job);
    m_completedJobsMutex.unlock();
}



//----------------------------------------------------------------------------------------------------------------------
Job* JobSystem::RemoveJobFromCompletedQueue(JobID jobID)
{
    Job* result = nullptr;
    m_completedJobsMutex.lock();
    for (int i = 0; i < (int) m_completedJobs.size(); ++i)
    {
        if (m_completedJobs[i]->m_id == jobID)
        {
            result = m_completedJobs[i];
            m_completedJobs.erase(m_completedJobs.begin() + i);
            break;
        }
    }
    m_completedJobsMutex.unlock();
    return result;
}