﻿// Bradley Christensen - 2022
#include "JobID.h"



JobID JobID::Invalid = { UINT32_MAX };



//----------------------------------------------------------------------------------------------------------------------
JobID::JobID() : m_uniqueID(UINT32_MAX)
{
    
}



//----------------------------------------------------------------------------------------------------------------------
JobID::JobID(uint32_t uniqueID) : m_uniqueID(uniqueID)
{
}



//----------------------------------------------------------------------------------------------------------------------
bool JobID::operator<(JobID const& rhs) const
{
    return (m_uniqueID < rhs.m_uniqueID);
}



//----------------------------------------------------------------------------------------------------------------------
bool JobID::operator==(JobID const& rhs) const
{
    return (m_uniqueID == rhs.m_uniqueID);
}



//----------------------------------------------------------------------------------------------------------------------
bool JobID::operator!=(JobID const& rhs) const
{
    return (m_uniqueID != rhs.m_uniqueID);
}
