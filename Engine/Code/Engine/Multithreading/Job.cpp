// Bradley Christensen - 2022
#include "Job.h"



//----------------------------------------------------------------------------------------------------------------------
bool Job::IsValid() const
{
    return m_id != JobID::Invalid;
}



//----------------------------------------------------------------------------------------------------------------------
bool Job::HasDependencies() const
{
    return m_dependencies.m_readDependencies != 0 || m_dependencies.m_writeDependencies != 0;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Job::GetIndex() const
{
    return m_id.m_index;
}



//----------------------------------------------------------------------------------------------------------------------
uint32_t Job::GetUniqueID() const
{
    return m_id.m_uniqueID;
}
