// Bradley Christensen - 2022
#include "JobGraph.h"
#include <algorithm>



//----------------------------------------------------------------------------------------------------------------------
void JobGraph::AddJob(Job* job)
{
    m_jobs.push_back(job);
    SortByPriority();

    job->m_needsComplete = true; // all jobs in a job graph must be completed
    job->m_deleteAfterCompletion = false; // job graphs may be recurring, so don't delete the jobs
}



//----------------------------------------------------------------------------------------------------------------------
bool JobGraph::IsComplete() const
{
    for (auto& status : m_jobStatuses)
    {
        if (status != JobStatus::Completed)
        {
            return false;
        }
    }
    return true;
}



//----------------------------------------------------------------------------------------------------------------------
void JobGraph::Initialize()
{
    m_jobReceipts.clear();
    m_jobStatuses.clear();
    m_jobDeps.clear();
    
    m_jobReceipts.resize(m_jobs.size(), JobID::Invalid);
    m_jobStatuses.resize(m_jobs.size(), JobStatus::Null);
    
    for (auto& job : m_jobs)
    {
        m_jobDeps.push_back(job->GetJobDependencies());
    }
}



//----------------------------------------------------------------------------------------------------------------------
bool SortJobsByPrioPredicate(Job const* lhs, Job const* rhs)
{
    return lhs->GetJobPriority() < rhs->GetJobPriority();
}



//----------------------------------------------------------------------------------------------------------------------
void JobGraph::SortByPriority()
{
    std::sort(m_jobs.begin(), m_jobs.end(), SortJobsByPrioPredicate);
} 