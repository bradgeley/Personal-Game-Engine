﻿// Bradley Christensen - 2022-2025
#include "JobWorker.h"



//----------------------------------------------------------------------------------------------------------------------
void JobWorker::Shutdown()
{
    m_isRunning = false;
    m_thread.join();
}
