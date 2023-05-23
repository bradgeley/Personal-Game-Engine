// Bradley Christensen - 2023
#pragma once
#include "Engine/Multithreading/JobGraph.h"
#include <vector>



class AdminSystem;
class System;



//----------------------------------------------------------------------------------------------------------------------
class SystemScheduler
{
public:

	SystemScheduler(AdminSystem* admin);
	~SystemScheduler() = default;

	void Schedule(std::vector<System*> const& systems);
	void RunMultithreaded(float deltaSeconds);
	void RunSinglethreaded(float deltaSeconds) const;

private:

	void Cleanup();

	AdminSystem* m_admin = nullptr;
	std::vector<System*> m_frameSystemsInOrder;
	JobGraph m_jobGraph;
};


