// Bradley Christensen - 2023
#pragma once
#include "EntityID.h"



class System;
class AdminSystem;



//----------------------------------------------------------------------------------------------------------------------
struct SystemContext
{
    SystemContext(AdminSystem* admin, System* system, float deltaSeconds);
    void SetSystemSplittingParams(int systemSplittingJobID, int systemSplittingNumJobs);
	
    AdminSystem*    m_admin                     = nullptr;
    System*         m_system                    = nullptr;
    float		    m_deltaSeconds				= 0.f;

    // System Splitting Params
    bool		    m_systemSplittingEnabled	= false;
    EntityID	    m_startEntityID				= 0;
    EntityID	    m_endEntityID				= MAX_ENTITIES - 1;
    int			    m_systemSplittingJobID		= 0; // Index ranging from 0 to (SystemSplittingNumJobs - 1)
};
