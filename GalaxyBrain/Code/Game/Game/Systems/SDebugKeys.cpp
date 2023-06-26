// Bradley Christensen - 2023
#include "SDebugKeys.h"
#include "Engine/Input/InputSystem.h"
#include "Game/Game/Singletons/SCEntityFactory.h"
#include <Engine/Multithreading/JobSystemDebug.h>



//----------------------------------------------------------------------------------------------------------------------
void SDebugKeys::Startup()
{
    AddWriteDependencies<SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugKeys::Run(SystemContext const& context)
{
    if (g_input->WasKeyJustPressed('0'))
    {
        if (!g_jobSystemDebug)
        {
            JobSystemDebugConfig jobSysDebugConfig;
            g_jobSystemDebug = new JobSystemDebug(jobSysDebugConfig);
            g_jobSystemDebug->Startup();
        }
        else
        {
            g_jobSystemDebug->Shutdown();
            delete g_jobSystemDebug;
            g_jobSystemDebug = nullptr;
        }
    }

    if (g_input->WasKeyJustReleased(' '))
    {
        g_ecs->DestroyAllEntities();

        SCEntityFactory* factory = g_ecs->GetComponent<SCEntityFactory>();

        SpawnInfo playerInfo;
        playerInfo.m_spawnPos = Vec2(50.f, 25.f);
        playerInfo.m_def = factory->GetEntityDef("Player");
        factory->m_entitiesToSpawn.emplace_back(playerInfo);

        SpawnInfo earthInfo;
        earthInfo.m_spawnPos = Vec2(0.f, 0.f);
        earthInfo.m_def = factory->GetEntityDef("Earth");
        factory->m_entitiesToSpawn.emplace_back(earthInfo);
    }
}



//----------------------------------------------------------------------------------------------------------------------
void SDebugKeys::Shutdown()
{
    if (g_jobSystemDebug)
    {
        g_jobSystemDebug->Shutdown();
        delete g_jobSystemDebug;
        g_jobSystemDebug = nullptr;
    }
}
