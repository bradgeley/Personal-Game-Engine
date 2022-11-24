// Bradley Christensen - 2022
#include "Engine.h"
#include "EngineSubsystem.h"



void Engine::Startup()
{
    for (EngineSubsystem*& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->Startup();
        }
    }
}



void Engine::BeginFrame()
{
    for (auto& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->BeginFrame();
        }
    }
}



void Engine::Update(float deltaSeconds)
{
    for (auto& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->Update(deltaSeconds);
        }
    } 
}



void Engine::Render()
{
    for (auto& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->Render();
        }
    }
}



void Engine::EndFrame()
{
    for (auto& subsystem : m_subsystems)
    {
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->EndFrame();
        }
    }
}



void Engine::Shutdown()
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem*& subsystem = m_subsystems[i];
        if (subsystem)
        {
            subsystem->Shutdown();
        }
        delete subsystem;
        subsystem = nullptr;
    }
    m_subsystems.clear();
}



void Engine::RegisterSubsystem(EngineSubsystem* system)
{
    m_subsystems.push_back(system);
}
