// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include <atomic>
#include <condition_variable>
#include <string>
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
// Job Worker
//
// Thin wrapper around std::thread
//
struct JobWorker
{
    void Shutdown();

    int                     m_threadID      = -1;
    std::atomic<bool>       m_isRunning     = true;
    Name                    m_name          = "Unnamed Worker";
    std::thread             m_thread;
    std::condition_variable m_condVar;
};