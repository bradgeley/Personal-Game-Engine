// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/Multithreading/JobSystem.h"



TEST(JobSystem, CreateDestroy)
{
    JobSystemConfig config;
    g_jobSystem = new JobSystem(config);
    EXPECT_TRUE(g_jobSystem != nullptr);
    delete g_jobSystem;
    g_jobSystem = nullptr;
    EXPECT_TRUE(g_jobSystem == nullptr);
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
        JobID job = g_jobSystem->PostJob(new IncIntJob(numToIncrement));
        g_jobSystem->WaitForJob(job);
    }
};
struct SimpleNeedsCompleteJob : public Job
{
    SimpleNeedsCompleteJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement) {}
    std::atomic<int>& numToIncrement;
    void Execute() override
    {
        numToIncrement++;
    }
    bool NeedsComplete() override { return true; }
    void Complete() override
    {
        numToIncrement--;
    }
};




TEST(JobSystem, JobID)
{
    JobSystemConfig config;
    //config.m_threadCount = 100;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();

    std::atomic testInt = 0;
    for (int i = 0; i < 100; ++i)
    {
        IncIntJob* newJob = new IncIntJob(testInt);
        JobID id = g_jobSystem->PostJob(newJob);
        EXPECT_EQ((int) id.m_uniqueID, i);
    }
    
    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



TEST(JobSystem, SimpleParallel)
{
    JobSystemConfig config;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);

    std::atomic testInt = 0;
    for (int i = 0; i < 100; ++i)
    {
        g_jobSystem->PostJob(new IncIntJob(testInt));
    }
    g_jobSystem->WaitForAllJobs();

    int value = testInt;
    EXPECT_EQ(value, 100);

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



TEST(JobSystem, SimpleRecursive)
{
    JobSystemConfig config;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);

    std::atomic testInt = 0;
    for (int i = 0; i < 100; ++i)
    {
        g_jobSystem->PostJob(new SimpleRecursiveJob(testInt));
    }
    g_jobSystem->WaitForAllJobs();

    int value = testInt; 
    EXPECT_EQ(value, 100);

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



TEST(JobSystem, SimpleNeedsComplete)
{
    JobSystemConfig config;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);

    std::atomic testInt = 0;
    constexpr int numJobs = 100;
    std::vector<JobID> ids;
    for (int i = 0; i < numJobs; ++i)
    {
        ids.push_back(g_jobSystem->PostJob(new SimpleNeedsCompleteJob(testInt)));
    }
    g_jobSystem->WaitForAllJobs();

    int value = testInt; 
    EXPECT_EQ(value, numJobs);

    g_jobSystem->CompleteJobs(ids);
    value = testInt; 
    EXPECT_EQ(value, 0);
    
    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}