// Bradley Christensen - 2022
#include "Engine/Multithreading/JobGraph.h"
#include "gtest/gtest.h"
#include "Engine/Multithreading/JobSystem.h"



//----------------------------------------------------------------------------------------------------------------------
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
struct IncIntJob : Job
{
    IncIntJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement) {}
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        ++numToIncrement;
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct SimpleRecursiveJob : Job
{
    SimpleRecursiveJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement) {}
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        auto id = g_jobSystem->PostJob(new IncIntJob(numToIncrement));
        g_jobSystem->WaitForJob(id);
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct SimpleNeedsCompleteJob : Job
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



//----------------------------------------------------------------------------------------------------------------------
struct SimpleRecursiveAndNeedsCompleteJob : Job
{
    SimpleRecursiveAndNeedsCompleteJob(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement) {}
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        JobID id = g_jobSystem->PostJob(new SimpleNeedsCompleteJob(numToIncrement));
        g_jobSystem->WaitForJob(id);
        g_jobSystem->CompleteJob(id);
    }
};



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, JobID)
{
    for (int t = 0; t < 100; ++t)
    {
        printf("JobID Test Starting...\n");
    
        JobSystemConfig config;
        config.m_enableLogging = true;
        g_jobSystem = new JobSystem(config);
        g_jobSystem->Startup();

        std::atomic testInt = 0;
        int numJobs = 1000;
    
        for (int i = 0; i < numJobs; ++i)
        {
            printf("Posting Job %i\n", i);
            IncIntJob* newJob = new IncIntJob(testInt);
            JobID id = g_jobSystem->PostJob(newJob);
            EXPECT_EQ((int) id.m_uniqueID, i); 
        }
    
        printf("Waiting for jobs...\n");
        g_jobSystem->WaitForAllJobs();
    
        int value = testInt;
        EXPECT_EQ(value, numJobs);
    
        g_jobSystem->Shutdown();
        delete g_jobSystem;
        g_jobSystem = nullptr;
    
        printf("Test Complete!\n");
    }
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleParallel)
{
    JobSystemConfig config;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);

    std::atomic testInt = 0;
    int numJobs = 1000;
    for (int i = 0; i < numJobs; ++i)
    {
        printf("Posting Job %i\n", i);
        g_jobSystem->PostJob(new IncIntJob(testInt));
    }
    g_jobSystem->WaitForAllJobs();

    int value = testInt;
    EXPECT_EQ(value, numJobs);

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleRecursive)
{
    JobSystemConfig config;
    //config.m_threadCount = 1;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);

    std::atomic testInt = 0;
    int numJobs = 1000;
    for (int i = 0; i < numJobs; ++i)
    {
        printf("Posting Job %i\n", i);
        g_jobSystem->PostJob(new SimpleRecursiveJob(testInt));
    }
    g_jobSystem->WaitForAllJobs();

    int value = testInt; 
    EXPECT_EQ(value, numJobs);

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleNeedsComplete)
{
    JobSystemConfig config;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);

    std::atomic testInt = 0;
    constexpr int numJobs = 1000;
    std::vector<JobID> ids;
    for (int i = 0; i < numJobs; ++i)
    {
        printf("Posting Job %i\n", i);
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



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, SimpleRecursiveAndNeedsComplete)
{
    JobSystemConfig config;
    //config.m_threadCount = 1;
    g_jobSystem = new JobSystem(config);
    g_jobSystem->Startup();
    EXPECT_TRUE(g_jobSystem != nullptr);

    std::atomic testInt = 0;
    int numJobs = 1000;
    for (int i = 0; i < numJobs; ++i)
    {
        printf("Posting Job %i\n", i);
        g_jobSystem->PostJob(new SimpleRecursiveAndNeedsCompleteJob(testInt));
    }
    g_jobSystem->WaitForAllJobs();

    int value = testInt; 
    EXPECT_EQ(value, 0);

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
struct JobWithDependencies_AddOne : Job
{
    JobWithDependencies_AddOne(std::atomic<int>& numToIncrement) : numToIncrement(numToIncrement) {}

    JobDependencies GetJobDependencies() const override
    {
        return { 0, 1 << 0 };
    }

    int GetJobPriority() const override
    {
        return 1;
    }
    
    std::atomic<int>& numToIncrement;
    
    void Execute() override
    {
        numToIncrement++;
    }

    bool NeedsComplete() override { return true; }
    
    void Complete() override
    {
        
    }
};



//----------------------------------------------------------------------------------------------------------------------
struct JobWithDependencies_MultiplyByTwo : Job
{
    JobWithDependencies_MultiplyByTwo(std::atomic<int>& num) : num(num) {}
    
    JobDependencies GetJobDependencies() const override
    {
        return { 0, 1 << 0 };
    }

    int GetJobPriority() const override
    {
        return 0;
    }
    
    std::atomic<int>& num;
    
    void Execute() override
    {
        num = num * 2;
    }

    bool NeedsComplete() override { return true; }
    
    void Complete() override
    {
        
    }
};



//----------------------------------------------------------------------------------------------------------------------
TEST(JobSystem, JobGraph)
{
    JobSystemConfig config;
    //config.m_threadCount = 1;
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

    g_jobSystem->ExecuteJobGraph(graph);

    int value = testInt; 
    EXPECT_EQ(value, 18);

    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}