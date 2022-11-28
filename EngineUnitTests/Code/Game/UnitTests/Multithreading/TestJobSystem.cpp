// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/Multithreading/JobSystem.h"



TEST(JobSystem, CreateDestroy)
{
    JobSystemConfig config;
    g_theJobSystem = new JobSystem(config);
    EXPECT_TRUE(g_theJobSystem != nullptr);
    delete g_theJobSystem;
    g_theJobSystem = nullptr;
    EXPECT_TRUE(g_theJobSystem == nullptr);
}


//----------------------------------------------------------------------------------------------------------------------
// Test Job
//
struct IncIntJob : Job
{
    IncIntJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement) {}
    std::atomic<int>& numToIncrement;
    void Execute() override
    {
        ++numToIncrement;
    }
};
struct SimpleRecursiveJob : public Job
{
    SimpleRecursiveJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement) {}
    std::atomic<int>& numToIncrement;
    void Execute() override
    {
        g_theJobSystem->QueueJob(new IncIntJob(numToIncrement));
    }
};




TEST(JobSystem, JobID)
{
    JobSystemConfig config;
    config.m_threadCount = 100;
    g_theJobSystem = new JobSystem(config);
    g_theJobSystem->Startup();

    std::atomic testInt = 0;
    for (int i = 0; i < 100; ++i)
    {
        IncIntJob* newJob = new IncIntJob(testInt);
        EXPECT_EQ((int) newJob->m_id, i);
    }
    
    g_theJobSystem->Shutdown();
    delete g_theJobSystem;
    g_theJobSystem = nullptr;
}



TEST(JobSystem, SimpleParallel)
{
    JobSystemConfig config;
    g_theJobSystem = new JobSystem(config);
    g_theJobSystem->Startup();
    EXPECT_TRUE(g_theJobSystem != nullptr);

    std::atomic testInt = 0;
    for (int i = 0; i < 100; ++i)
    {
        g_theJobSystem->QueueJob(new IncIntJob(testInt));
    }
    g_theJobSystem->WaitForAllJobs();

    int value = testInt;
    EXPECT_EQ(value, 100);

    g_theJobSystem->Shutdown();
    delete g_theJobSystem;
    g_theJobSystem = nullptr;
}



TEST(JobSystem, SimpleRecursive)
{
    JobSystemConfig config;
    g_theJobSystem = new JobSystem(config);
    g_theJobSystem->Startup();
    EXPECT_TRUE(g_theJobSystem != nullptr);

    std::atomic testInt = 0;
    for (int i = 0; i < 100; ++i)
    {
        g_theJobSystem->QueueJob(new SimpleRecursiveJob(testInt));
    }
    g_theJobSystem->WaitForAllJobs();

    int value = testInt; 
    EXPECT_EQ(value, 100);

    g_theJobSystem->Shutdown();
    delete g_theJobSystem;
    g_theJobSystem = nullptr;
}