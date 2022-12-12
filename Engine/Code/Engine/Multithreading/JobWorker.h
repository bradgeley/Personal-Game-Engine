﻿// Bradley Christensen - 2022
#pragma once
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
    
    std::atomic<bool>       m_isRunning     = true;
    std::string             m_name          = "Unnamed Worker";
    std::thread             m_thread;
    std::condition_variable m_condVar;
};