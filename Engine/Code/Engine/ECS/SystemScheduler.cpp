// Bradley Christensen - 2022-2025
#include "SystemScheduler.h"
#include "System.h"
#include "Engine/Multithreading/JobSystem.h"
#include "Engine/Performance/PerformanceDebugWindow.h"
#include "Engine/Multithreading/JobGraph.h"
#include "AdminSystem.h"
#include "Config.h"
#include "SystemSubgraph.h"
#include "Engine/Core/Time.h"
#include <algorithm>



//----------------------------------------------------------------------------------------------------------------------
static std::string s_ecsSectionName = "ECS";



//----------------------------------------------------------------------------------------------------------------------
void RunSystem(SystemContext const& context);
void SplitSystem(SystemContext const& context, int numJobs);



//----------------------------------------------------------------------------------------------------------------------
// Calls PreRun, Run (or splits system into multiple RunSystemJob), and PostRun on a system
//
struct AutoMultithreadedRunSystemJob : public Job
{
public:

	explicit AutoMultithreadedRunSystemJob(SystemContext const& context) : m_context(context)
	{
		m_priority = context.m_system->GetLocalPriority();
		m_jobDependencies.m_readDependencies = context.m_system->GetReadDependencies();
		m_jobDependencies.m_writeDependencies = context.m_system->GetWriteDependencies();
	}

	virtual void Execute() override
	{
		RunSystem(m_context);
	}

protected:

	SystemContext m_context;
};



//----------------------------------------------------------------------------------------------------------------------
// Calls Run on a system
//
struct SplitSystemJob : public Job
{
public:

	explicit SplitSystemJob(SystemContext const& context) : m_context(context)
	{
		m_priority = context.m_system->GetLocalPriority();
		m_jobDependencies.m_readDependencies = context.m_system->GetReadDependencies();
		m_jobDependencies.m_writeDependencies = context.m_system->GetWriteDependencies();
	}

	virtual void Execute() override
	{
		m_context.m_system->Run(m_context);
	}

protected:

	SystemContext m_context;
};



//----------------------------------------------------------------------------------------------------------------------
SystemScheduler::SystemScheduler(AdminSystem* admin) : g_ecs(admin)
{

}



//----------------------------------------------------------------------------------------------------------------------
static bool SystemPriorityComparator(System* a, System* b) 
{
	return (a->GetLocalPriority()) < (b->GetLocalPriority());
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::ScheduleFrame(std::vector<SystemSubgraph>& systems)
{
	Cleanup();

	for (SystemSubgraph& subgraph : systems)
	{
		m_systemSubgraphs.push_back(&subgraph);
	}
	
	// Within each subgraph, sort each system by priority
	for (SystemSubgraph* subgraph : m_systemSubgraphs)
	{
		std::sort(subgraph->m_systems.begin(), subgraph->m_systems.end(), SystemPriorityComparator);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::RunFrame(float deltaSeconds)
{
	if (g_jobSystem && g_ecs->IsAutoMultithreadingActive())
	{
		RunFrame_AutoMultithreaded(deltaSeconds);
	}
	else
	{
		RunFrame_Singlethreaded(deltaSeconds);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::RunSubgraph(SystemSubgraph const& subgraph, float deltaSeconds) const
{
	SystemSubgraph copy = subgraph;
	std::sort(copy.m_systems.begin(), copy.m_systems.end(), SystemPriorityComparator);
	
	bool multithreaded = g_jobSystem && g_ecs->IsAutoMultithreadingActive();
	TryRunSubgraph(copy, deltaSeconds, multithreaded);
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::RunFrame_AutoMultithreaded(float deltaSeconds)
{
	for (SystemSubgraph* subgraph : m_systemSubgraphs)
	{
		TryRunSubgraph(*subgraph, deltaSeconds, true);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::RunFrame_Singlethreaded(float deltaSeconds)
{
	for (SystemSubgraph* subgraph : m_systemSubgraphs)
	{
		TryRunSubgraph(*subgraph, deltaSeconds, false);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::TryRunSubgraph(SystemSubgraph& subgraph, float deltaSeconds, bool multithreaded) const
{
	if (subgraph.m_timeStep >= SYSTEM_MIN_TIME_STEP)
	{
		subgraph.m_accumulatedTime += deltaSeconds;
		while (subgraph.m_accumulatedTime > subgraph.m_timeStep)
		{
			subgraph.m_accumulatedTime -= subgraph.m_timeStep;

			if (multithreaded)
			{
				RunSubgraph_AutoMultithreaded(subgraph, subgraph.m_timeStep);
			}
			else RunSubgraph_Singlethreaded(subgraph, subgraph.m_timeStep);
		}
	}
	else
	{
		// Just call using deltaSeconds
		if (multithreaded)
		{
			RunSubgraph_AutoMultithreaded(subgraph, deltaSeconds);
		}
		else RunSubgraph_Singlethreaded(subgraph, deltaSeconds);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void RunSystem(SystemContext const& context)
{
	PerfItemData perfItem;
	perfItem.m_tint = context.m_system->GetDebugTint();
	perfItem.m_startTime = GetCurrentTimeSeconds();

	context.m_system->PreRun();
	
	int systemSplittingNumJobs = context.m_system->GetSystemSplittingNumJobs();
	if (g_ecs->IsSystemSplittingActive() && systemSplittingNumJobs > 1)
	{
		SplitSystem(context, systemSplittingNumJobs);
	}
	else
	{
		context.m_system->Run(context);
	}
	
	context.m_system->PostRun();

	perfItem.m_endTime = GetCurrentTimeSeconds();

	if (g_performanceDebugWindow)
	{
		g_performanceDebugWindow->LogItem(perfItem, s_ecsSectionName, context.m_system->GetName());
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SplitSystem(SystemContext const& context, int numJobs)
{
	// Split system into multiple run jobs (this can be significantly slower than single threaded if the entity count is low
	std::vector<JobID> jobReceipts(numJobs);

	for (int systemSplittingJobID = 0; systemSplittingJobID < numJobs; ++systemSplittingJobID)
	{
		// Copy the context, but split the entities amongst them
		SystemContext splitContext = context;
		splitContext.SplitEntities(systemSplittingJobID, numJobs);
		
		Job* job = new SplitSystemJob(splitContext);
		jobReceipts[systemSplittingJobID] = g_jobSystem->PostJob(job);
	}

	// Block until all the split jobs are complete
	g_jobSystem->CompleteJobs(jobReceipts);
}



//----------------------------------------------------------------------------------------------------------------------
// No job graph, just run each system (allowing splitting) one by one
//
void SystemScheduler::RunSubgraph_Singlethreaded(SystemSubgraph const& subgraph, float deltaSeconds) const
{
	for (System* const& system : subgraph.m_systems)
	{
		if (!system->IsActive())
		{
			continue;
		}

		SystemContext context(system, deltaSeconds);

		PerfItemData perfItem;
		perfItem.m_tint = system->GetDebugTint();
		perfItem.m_startTime = GetCurrentTimeSeconds();

		RunSystem(context);

		perfItem.m_endTime = GetCurrentTimeSeconds();

		if (g_performanceDebugWindow)
		{
			g_performanceDebugWindow->LogItem(perfItem, s_ecsSectionName, system->GetName());
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
// Uses a job graph to run a single system subgraph
// - so system calls within that subgraph can happen simultaneously
//
void SystemScheduler::RunSubgraph_AutoMultithreaded(SystemSubgraph const& subgraph, float deltaSeconds) const
{
	JobGraph jobGraph;
	jobGraph.Reserve(subgraph.m_systems.size());
	
	for (System* const& system : subgraph.m_systems)
	{
		if (!system->IsActive())
		{
			continue;
		}

		SystemContext context(system, deltaSeconds);
		Job* job = new AutoMultithreadedRunSystemJob(context);
		jobGraph.AddJob(job);
	}
	g_jobSystem->ExecuteJobGraph(jobGraph);

	jobGraph.Cleanup();
}



//----------------------------------------------------------------------------------------------------------------------
void SystemScheduler::Cleanup()
{
	m_systemSubgraphs.clear();
}