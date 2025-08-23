// Bradley Christensen - 2023
#pragma once
#include <vector>



class AdminSystem;
class System;
class SystemSubgraph;



//----------------------------------------------------------------------------------------------------------------------
class SystemScheduler
{
public:

	explicit SystemScheduler(AdminSystem* admin);

	void ScheduleFrame(std::vector<SystemSubgraph>& systems);
	void RunFrame(float deltaSeconds);
	void RunSubgraph(SystemSubgraph const& subgraph, float deltaSeconds) const;

private:

	void RunFrame_AutoMultithreaded(float deltaSeconds);
	void RunFrame_Singlethreaded(float deltaSeconds);

	// Runs a system 0+ times based on its time step params
	void TryRunSubgraph(SystemSubgraph& subgraph, float deltaSeconds, bool multithreaded) const;
	
	void RunSubgraph_Singlethreaded(SystemSubgraph const& subgraph, float deltaSeconds) const;
	void RunSubgraph_AutoMultithreaded(SystemSubgraph const& subgraph, float deltaSeconds) const;
	
	void Cleanup();

	AdminSystem* g_ecs = nullptr;
	std::vector<SystemSubgraph*> m_systemSubgraphs;
};
