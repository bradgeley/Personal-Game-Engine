﻿// Bradley Christensen - 2022
#include "Job.h"



//----------------------------------------------------------------------------------------------------------------------
bool Job::IsValid() const
{
    return m_id != JobID::Invalid;
}



//----------------------------------------------------------------------------------------------------------------------
bool Job::HasDependencies() const
{
    JobDependencies const& deps = GetJobDependencies();
    return deps.m_readDependencies != 0 || deps.m_writeDependencies != 0;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Job::GetUniqueID() const
{
    return m_id.m_uniqueID;
}
