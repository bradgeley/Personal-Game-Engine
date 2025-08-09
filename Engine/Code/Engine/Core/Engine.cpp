﻿// Bradley Christensen - 2022-2023
#include "Engine.h"
#include "EngineCommon.h"
#include "EngineSubsystem.h"
#include "Engine/Performance/ScopedTimer.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
// THE ENGINE
//
Engine* g_engine = nullptr;



//----------------------------------------------------------------------------------------------------------------------
void Engine::Startup()
{
    if (m_isActive)
    {
        return;
    }

    for (EngineSubsystem*& subsystem : m_subsystems)
    {
        if (subsystem)
        {
            subsystem->Startup();
        }
    }

    m_isActive = true;
}



//----------------------------------------------------------------------------------------------------------------------
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



//----------------------------------------------------------------------------------------------------------------------
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



//----------------------------------------------------------------------------------------------------------------------
void Engine::Render() const
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem* const& subsystem = m_subsystems[i];
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->Render();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem*& subsystem = m_subsystems[i];
        if (subsystem && subsystem->IsEnabled())
        {
            subsystem->EndFrame();
        }
    }
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::Shutdown()
{
    for (int i = (int) m_subsystems.size() - 1; i >= 0; --i)
    {
        EngineSubsystem*& subsystem = m_subsystems[i];
        if (subsystem)
        {
            SHUTDOWN_AND_DESTROY(subsystem)
        }
    }
    m_subsystems.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void Engine::RegisterSubsystem(EngineSubsystem* system)
{
    if (m_isActive)
    {
        return;
    }

    m_subsystems.push_back(system);
}
