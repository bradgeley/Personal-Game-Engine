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
    for (auto& system : m_systems)
    {
        system->Shutdown();
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SystemSubgraph::Cleanup()
{
    for (auto& system : m_systems)
    {
        delete system;
    }
    m_systems.clear();
}
