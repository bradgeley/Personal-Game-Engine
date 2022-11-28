// Bradley Christensen - 2022
#pragma once
#include <atomic>
#include <string>
#include <thread>



//----------------------------------------------------------------------------------------------------------------------
// Job Worker
//
// Thin wrapper around std::thread
//
struct JobWorker
{
    std::atomic<bool>   m_isRunning     = true;
    std::string         m_name          = "Unnamed Worker";
    std::thread         m_thread;
};