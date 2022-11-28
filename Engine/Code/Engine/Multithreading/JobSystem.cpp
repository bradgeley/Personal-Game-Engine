// Bradley Christensen - 2022
#include "Engine/Multithreading/JobSystem.h"
#include "JobWorker.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
// THE JOB SYSTEM
//
JobSystem* g_theJobSystem = nullptr;



JobSystem::JobSystem(JobSystemConfig const& config) : m_config(config)
{

}



void JobSystem::QueueJob(Job* job)
{
    job->m_status = JobStatus::WaitingToStart;
    
    m_jobQueueMutex.lock();
    for (int i = 0; i < (int) m_jobQueue.size(); ++i)
    {
        if (m_jobQueue[i] == nullptr)
        {
            m_jobQueue[i] = job;
            break;
        }
    }
    m_jobQueue.emplace_back(job);
    m_jobQueueMutex.unlock();
    
    m_jobCondVar.notify_one();
}



void JobSystem::WaitForAllJobs()
{
    while (m_isRunning)
    {
        int numCompleteJobs = 0;
        
        m_jobQueueMutex.lock();
        int numJobsQueued = (int) m_jobQueue.size();
        for (int i = 0; i < numJobsQueued; ++i)
        {
            if (m_jobQueue[i]->m_status == JobStatus::Complete)
            {
                numCompleteJobs++;
            }
            else break;
        }
        m_jobQueueMutex.unlock();
        
        if (numCompleteJobs == numJobsQueued)
        {
            // break the infinite loop finally
            return;
        }
        
        std::this_thread::yield();
    }
}



void JobSystem::Startup()
{
    EngineSubsystem::Startup();

    for (int i = 0; i < (int) m_config.m_threadCount; ++i)
    {
        JobWorker* worker = new JobWorker();
        worker->m_thread = std::thread(&JobSystem::WorkerLoop, this, worker );
        worker->m_name = StringF("JobSystemWorker: %i", i);
        m_workers.emplace_back( worker );
    }
}



void JobSystem::Shutdown()
{
    EngineSubsystem::Shutdown();

    WaitForAllJobs();
    
    m_isRunning = false;
    m_jobCondVar.notify_all();

    for (int i = 0; i < (int) m_workers.size(); ++i)
    {
        m_workers[i]->m_isRunning = false;
        m_workers[i]->m_thread.join();
        delete m_workers[i];
        m_workers[i] = nullptr;
        m_workers.erase(m_workers.begin() + i);
    }
    m_workers.clear();

    // no need to grab the mutex since all threads are dead but doing it anyway
    m_jobQueueMutex.lock();
    for (int i = 0; i < (int) m_jobQueue.size(); ++i)
    {
        if (m_jobQueue[i] != nullptr)
        {
            delete m_jobQueue[i];
            m_jobQueue[i] = nullptr;
        }
    }
    m_jobQueue.clear();
    m_jobQueueMutex.unlock();
}



JobID JobSystem::GetNextJobID()
{
    return g_theJobSystem->m_nextJobID.fetch_add(1);
}



void JobSystem::WorkerLoop(JobWorker* worker)
{
    while (m_isRunning && worker->m_isRunning)
    {
        Job* job = GetNextJob();
        if (job)
        {
            job->Execute();

            m_jobQueueMutex.lock();
            job->m_status = JobStatus::Complete;
            m_jobQueueMutex.unlock();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}



Job* JobSystem::GetNextJob()
{
    Job* result = nullptr;
    std::unique_lock uniqueLock( m_jobQueueMutex );
    while ( m_isRunning && m_jobQueue.size() == 0 )
    {
        m_jobCondVar.wait( uniqueLock );
    }
    if ( m_jobQueue.size() != 0 )
    {
        for (int i = 0; i < (int) m_jobQueue.size(); ++i)
        {
            Job*& job = m_jobQueue[i];
            if (job && job->m_status == JobStatus::WaitingToStart)
            {
                result = job;
                job->m_status = JobStatus::InProgress;
                break;
            }
        }
    }

    return result;
}
