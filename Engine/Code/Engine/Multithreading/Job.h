// Bradley Christensen - 2022
#pragma once
#include <condition_variable>
#include <cstdint>



typedef uint64_t JobID;



enum class JobStatus : uint8_t
{
    Invalid,
    WaitingToStart,
    InProgress,
    Complete,
};



//----------------------------------------------------------------------------------------------------------------------
// Job
//
// A unit of work, executed on an arbitrary thread
//
struct Job
{
    Job();
    virtual ~Job() = default;
    virtual void Execute() {}

    JobID const m_id;
    JobStatus m_status              = JobStatus::Invalid;
    uint64_t m_frameNumber          = 0;
    uint64_t m_readDependencies     = 0;
    uint64_t m_writeDependencies    = 0;

    std::condition_variable m_condVar;
};