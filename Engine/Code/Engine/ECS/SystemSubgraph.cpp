// Bradley Christensen - 2023
#include "SystemSubgraph.h"
#include "System.h"



//----------------------------------------------------------------------------------------------------------------------
void SystemSubgraph::Startup() const
{
    for (auto& system : m_systems)
    {
        system->Startup();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SystemSubgraph::Shutdown() const
{
    for (int i = (int) m_systems.size() - 1; i >= 0; --i)
    {
        m_systems[i]->Shutdown();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SystemSubgraph::Cleanup()
{
    for (auto& system : m_systems)
    {
        delete system;
        system = nullptr;
    }
    m_systems.clear();
}
