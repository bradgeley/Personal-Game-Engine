// Bradley Christensen - 2022-2025
#include "Job.h"



//----------------------------------------------------------------------------------------------------------------------
int Job::GetJobPriority() const
{
    return m_priority;
}



//----------------------------------------------------------------------------------------------------------------------
bool Job::GetNeedsComplete() const
{
    return m_needsComplete;
}



//----------------------------------------------------------------------------------------------------------------------
bool Job::GetDeleteAfterCompletion() const
{
    return m_deleteAfterCompletion;
}



//----------------------------------------------------------------------------------------------------------------------
JobDependencies const& Job::GetJobDependencies() const
{
    return m_jobDependencies;
}



//----------------------------------------------------------------------------------------------------------------------
void Job::SetNeedsComplete(bool needsComplete)
{
    m_needsComplete = needsComplete;
}



//----------------------------------------------------------------------------------------------------------------------
void Job::SetDeleteAfterCompletion(bool deleteAfterCompletion)
{
    m_deleteAfterCompletion = deleteAfterCompletion;
}



//----------------------------------------------------------------------------------------------------------------------
void Job::SetPriority(int priority)
{
	m_priority = priority;  
}



//----------------------------------------------------------------------------------------------------------------------
bool Job::IsValid() const
{
    return m_id != JobID::Invalid;
}



//----------------------------------------------------------------------------------------------------------------------
bool Job::HasDependencies() const
{
    JobDependencies deps = GetJobDependencies();
    return deps.m_readDependencies != 0 || deps.m_writeDependencies != 0;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Job::GetUniqueID() const
{
    return m_id.m_uniqueID;
}



//----------------------------------------------------------------------------------------------------------------------
bool Job::operator<(Job const& rhs) const
{
    return m_priority < rhs.m_priority;
}



//----------------------------------------------------------------------------------------------------------------------
void Job::Complete()
{
    // Empty
}