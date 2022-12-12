﻿// Bradley Christensen - 2022
#include "Job.h"



bool Job::IsValid() const
{
    return m_id != JobID::Invalid;
}



uint32_t Job::GetIndex() const
{
    return m_id.m_index;
}



uint32_t Job::GetUniqueID() const
{
    return m_id.m_uniqueID;
}
