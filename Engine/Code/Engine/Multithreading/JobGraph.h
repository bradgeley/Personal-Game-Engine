// Bradley Christensen - 2022
#pragma once
#include <vector>



class Job;



//----------------------------------------------------------------------------------------------------------------------
// Job Graph
//
// A graph of jobs where each job will complete as it becomes available for it to do so
//
struct JobGraph
{
    std::vector<Job*> m_jobs;
};