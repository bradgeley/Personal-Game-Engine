// Bradley Christensen - 2023
#include "SystemScheduler.h"
#include "System.h"
#include "Engine/Multithreading/JobSystem.h"
#include "AdminSystem.h"
#include "Config.h"



//----------------------------------------------------------------------------------------------------------------------
void RunSystem(SystemContext const& context);



//----------------------------------------------------------------------------------------------------------------------
// System Update Task
//
struct MultithreadedSystemUpdateJob : public Job
{
public:

	explicit MultithreadedSystemUpdateJob(SystemContext const& context) : m_context(context) {}

	virtual void Execute() override
	{
		RunSystem(m_context);
	}

protected:

	SystemContext m_context;
};



//----------------------------------------------------------------------------------------------------------------------
struct RunSystemJob : public MultithreadedSystemUpdateJob
{
public:

	explicit RunSystemJob(SystemContext const& context) : MultithreadedSystemUpdateJob(context) {}

	virtual void Execute() override
	{
		m_context.m_system->Run(m_context);
	}
};



//----------------------------------------------------------------------------------------------------------------------
SystemScheduler::SystemScheduler(AdminSystem* admin) : m_admin(admin)
{

}



//----------------------------------------------------------------------------------------------------------------------
static bool SystemPriorityComparator(System* a, System* b) 
{
	return (a->GetPriority()) < (b->GetPriority());
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::Schedule(std::vector<System*> const& systems)
{
	Cleanup();

	for (System* const& s : systems)
	{
		if (s->IsActive())
		{
			m_frameSystemsInOrder.push_back(s);
		}
	}

	// need to sort by priority just in case user rearranges priority in the system window
	// or registers systems not in the order of their priority
	std::sort(m_frameSystemsInOrder.begin(), m_frameSystemsInOrder.end(), SystemPriorityComparator);
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::RunMultithreaded(float deltaSeconds)
{
	for (System* const& system : m_frameSystemsInOrder)
	{
		// If system splitting is active, this task will spawn multiple tasks and collect them before completing
		SystemContext context(m_admin, system, deltaSeconds);
		context.m_systemSplittingEnabled = m_admin->IsSystemSplittingActive();
		m_jobGraph.AddJob(new MultithreadedSystemUpdateJob(context));
	}
	
	g_jobSystem->ExecuteJobGraph(m_jobGraph);
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::RunSinglethreaded(float deltaSeconds) const
{
	for (System* const& system : m_frameSystemsInOrder)
	{
		SystemContext context(m_admin, system, deltaSeconds);
		context.m_systemSplittingEnabled = m_admin->IsSystemSplittingActive();
		RunSystem(context);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunSystem(SystemContext const& context)
{
	int systemSplittingNumJobs = context.m_system->GetSystemSplittingNumJobs();

	context.m_system->PreRun();
	
	if (!context.m_systemSplittingEnabled || systemSplittingNumJobs == 1)
	{
		context.m_system->Run(context);
	}
	else
	{
		std::vector<JobID> jobReceipts(systemSplittingNumJobs);

		for (int systemSplittingJobID = 0; systemSplittingJobID < systemSplittingNumJobs; ++systemSplittingJobID)
		{
			SystemContext splitContext = context;
			splitContext.SetSystemSplittingParams(systemSplittingJobID, systemSplittingNumJobs);
			
			Job* job = new RunSystemJob(splitContext);
			jobReceipts[systemSplittingJobID] = g_jobSystem->PostJob(job);
		}
		
		g_jobSystem->CompleteJobs(jobReceipts);
	}

	context.m_system->PostRun();
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::Cleanup()
{
	m_frameSystemsInOrder.clear();
	m_jobGraph.Shutdown();
}