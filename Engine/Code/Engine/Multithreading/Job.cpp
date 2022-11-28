// Bradley Christensen - 2022
#include "Job.h"
#include "JobSystem.h"



Job::Job() : m_id(JobSystem::GetNextJobID())
{
}
