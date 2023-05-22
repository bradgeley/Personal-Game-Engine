﻿// Bradley Christensen - 2022
#include "Engine/Multithreading/JobGraph.h"
#include "gtest/gtest.h"
#include "Engine/Multithreading/JobSystem.h"



int JOBSYSTEM_TEST_NUM_THREADS = 32;
int JOBSYSTEM_TEST_NUM_JOBS = 1000;
int JOBSYSTEM_TEST_NUM_ITERATIONS = 10;



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, CreateDestroy)
{
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        JobSystemConfig config;
        config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
        g_jobSystem = new JobSystem(config);
        EXPECT_TRUE(g_jobSystem != nullptr);
        delete g_jobSystem;
        g_jobSystem = nullptr;
        EXPECT_TRUE(g_jobSystem == nullptr);
    }
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, StartupShutdown)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    g_jobSystem = new JobSystem(config);
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        g_jobSystem->Startup();
        g_jobSystem->Shutdown();
    }
    
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
struct IncIntJob : Job
{
    IncIntJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement)
    {
        m_needsComplete = false;
    }
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        ++numToIncrement;
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct SimpleRecursiveJob : Job
{
    SimpleRecursiveJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement)
    {
        m_needsComplete = false;
    }
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        g_jobSystem->PostJob(new IncIntJob(numToIncrement));
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct SimpleNeedsCompleteJob : Job
{
    SimpleNeedsCompleteJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement)
    {
        m_needsComplete = true;
    }
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        numToIncrement++;
    }
    
    void Complete() override
    {
        numToIncrement--;
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct SimpleRecursiveAndNeedsCompleteJob : Job
{
    SimpleRecursiveAndNeedsCompleteJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement)
    {
        m_needsComplete = false;
    }
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        JobID id = g_jobSystem->PostJob(new SimpleNeedsCompleteJob(numToIncrement));
        g_jobSystem->CompleteJob(id);
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct RecursiveAndNeedsThreeCompletesJob : Job
{
    RecursiveAndNeedsThreeCompletesJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement)
    {
        m_needsComplete = false;
    }
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        std::vector<JobID> jobs;
        jobs.resize(3);
        jobs[0] = g_jobSystem->PostJob(new SimpleNeedsCompleteJob(numToIncrement));
        jobs[1] = g_jobSystem->PostJob(new SimpleNeedsCompleteJob(numToIncrement));
        jobs[2] = g_jobSystem->PostJob(new SimpleNeedsCompleteJob(numToIncrement));
        g_jobSystem->CompleteJobs(jobs);
    }
};



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, JobID)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    config.m_enableLogging = true;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        std::atomic testInt = 0;
        std::set<JobID> ids;
        
        for (int i = 0; i < JOBSYSTEM_TEST_NUM_JOBS; ++i)
        {
            IncIntJob* newJob = new IncIntJob(testInt);
            ids.emplace(g_jobSystem->PostJob(newJob));
        }

        // in a set all values are unique, so as long as all the values in the list are unique, we pass the test
        EXPECT_TRUE((int) ids.size() == JOBSYSTEM_TEST_NUM_JOBS);
    
        g_jobSystem->WaitForAllJobs();
    
        int value = testInt;
        EXPECT_EQ(value, JOBSYSTEM_TEST_NUM_JOBS);
    }
    
    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleParallel)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        std::atomic testInt = 0;
        for (int i = 0; i < JOBSYSTEM_TEST_NUM_JOBS; ++i)
        {
            g_jobSystem->PostJob(new IncIntJob(testInt));
        }
        g_jobSystem->WaitForAllJobs();

        int value = testInt;
        EXPECT_EQ(value, JOBSYSTEM_TEST_NUM_JOBS);
    }

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleRecursive)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        std::atomic testInt = 0;
        for (int i = 0; i < JOBSYSTEM_TEST_NUM_JOBS; ++i)
        {
            g_jobSystem->PostJob(new SimpleRecursiveJob(testInt));
        }
        g_jobSystem->WaitForAllJobs();

        int value = testInt; 
        EXPECT_EQ(value, JOBSYSTEM_TEST_NUM_JOBS);
    }
    
    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleNeedsComplete)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        std::atomic testInt = 0;
        std::vector<JobID> ids;
        for (int j = 0; j < JOBSYSTEM_TEST_NUM_JOBS; ++j)
        {
            ids.emplace_back(g_jobSystem->PostJob(new SimpleNeedsCompleteJob(testInt)));
        }
    
        g_jobSystem->CompleteJobs(ids);

        int value = testInt; 
        EXPECT_EQ(value, 0);
    }

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleRecursiveAndNeedsComplete)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        std::atomic testInt = 0;
        for (int i = 0; i < JOBSYSTEM_TEST_NUM_JOBS; ++i)
        {
            g_jobSystem->PostJob(new SimpleRecursiveAndNeedsCompleteJob(testInt));
        }

        g_jobSystem->WaitForAllJobs();

        int value = testInt; 
        EXPECT_EQ(value, 0);
    }

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleRecursiveAndNeedsMultipleCompletes)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        std::atomic testInt = 0;
        for (int i = 0; i < JOBSYSTEM_TEST_NUM_JOBS; ++i)
        {
            g_jobSystem->PostJob(new RecursiveAndNeedsThreeCompletesJob(testInt));
        }

        g_jobSystem->WaitForAllJobs();

        int value = testInt; 
        EXPECT_EQ(value, 0);
    }
    
    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
struct JobWithDependencies_AddOne : Job
{
    JobWithDependencies_AddOne(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement)
    {
        m_needsComplete = false;
        m_priority = 1;
        m_jobDependencies = { 0, 1 << 0 };
    }
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        numToIncrement++;
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct JobWithDependencies_MultiplyByTwo : Job
{
    JobWithDependencies_MultiplyByTwo(std::atomic<int>& num) : num(num)
    {
        m_needsComplete = false;
        m_priority = 0;
        m_jobDependencies = { 0, 1 << 0 };
    }
    
    std::atomic<int>& num;
    
    void Execute() override
    {
        num = num * 2;
    }
};



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, JobGraph)
{
    JobSystemConfig config;
    config.m_threadCount = JOBSYSTEM_TEST_NUM_THREADS;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);
    
    std::atomic testInt = 1;
    
    JobGraph graph;
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_MultiplyByTwo(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_MultiplyByTwo(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_MultiplyByTwo(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    graph.AddJob(new JobWithDependencies_AddOne(testInt));
    
    for (int t = 0; t < JOBSYSTEM_TEST_NUM_ITERATIONS; ++t)
    {
        testInt = 1;
        
        g_jobSystem->ExecuteJobGraph(graph, true);

        int value = testInt; 
        EXPECT_EQ(value, 18);
    }

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}