// Bradley Christensen - 2023
#pragma once
#include "EntityID.h"



class System;
class AdminSystem;



//----------------------------------------------------------------------------------------------------------------------
struct SystemContext
{
    explicit SystemContext(System* system, float deltaSeconds);

    // Sets start and end entity based on which job this system context is out of the total jobs
    void SplitEntities(int systemSplittingJobID, int systemSplittingNumJobs);
	
    System*         m_system                    = nullptr;
    float		    m_deltaSeconds				= 0.f;

    // System Splitting Params
    bool		    m_didSystemSplit	        = false;
    EntityID	    m_startEntityID				= 0;
    EntityID	    m_endEntityID				= MAX_ENTITIES - 1;
    int			    m_systemSplittingJobID		= 0; // Index ranging from 0 to (SystemSplittingNumJobs - 1)
    int             m_systemSplittingNumJobs    = 0;
};
