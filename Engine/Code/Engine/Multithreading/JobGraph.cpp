// Bradley Christensen - 2022
#include "JobGraph.h"
#include <algorithm>



//----------------------------------------------------------------------------------------------------------------------
void JobGraph::AddJob(Job* job)
{
    m_jobs.push_back(job);
    SortByPriority();

    // all jobs in job graph need complete
    job->m_needsComplete = true;
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
    
    m_jobReceipts.resize(m_jobs.size(), JobID::Invalid);
    m_jobStatuses.resize(m_jobs.size(), JobStatus::Null);
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