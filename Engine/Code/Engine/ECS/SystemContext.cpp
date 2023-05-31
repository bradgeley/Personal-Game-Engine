﻿// Bradley Christensen - 2023
#include "SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
SystemContext::SystemContext(System* system, float deltaSeconds) : m_system(system), m_deltaSeconds(deltaSeconds)
{
}



//----------------------------------------------------------------------------------------------------------------------
void SystemContext::SplitEntities(int systemSplittingJobID, int systemSplittingNumJobs)
{
    m_didSystemSplit = true;
    m_systemSplittingJobID = systemSplittingJobID;
    m_systemSplittingNumJobs = systemSplittingNumJobs;
    
    int startIndex = (int) MAX_ENTITIES * systemSplittingJobID / systemSplittingNumJobs;
    int endIndex = (int) MAX_ENTITIES * (systemSplittingJobID + 1) / systemSplittingNumJobs - 1;
    if (systemSplittingJobID == systemSplittingNumJobs - 1)
    {
        endIndex = MAX_ENTITIES - 1;
    }
    
    m_startEntityID = startIndex;
    m_endEntityID = endIndex;
}
