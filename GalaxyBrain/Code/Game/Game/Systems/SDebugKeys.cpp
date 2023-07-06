// Bradley Christensen - 2023
#include "SDebugKeys.h"
#include "Engine/Input/InputSystem.h"
#include "Game/Game/Singletons/SCEntityFactory.h"
#include "Game/Game/Singletons/SCDebug.h"
#include "Engine/Multithreading/JobSystemDebug.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugKeys::Startup()
{
    AddWriteDependencies<SCEntityFactory, SCDebug>();
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

    if (g_input->WasKeyJustPressed('1'))
    {
        //g_ecs->SetAllMultithreadingSettings(false, 0, 0);
        SCDebug* debug = g_ecs->GetComponent<SCDebug>();
        debug->m_physicsDebugDraw = !debug->m_physicsDebugDraw;
    }

    if (g_input->WasKeyJustReleased(' '))
    {
        g_ecs->DestroyAllEntities();

        SCEntityFactory* factory = g_ecs->GetComponent<SCEntityFactory>();

        SpawnInfo playerInfo;
        playerInfo.m_spawnPos = Vec2(50.f, 25.f);
        playerInfo.m_def = factory->GetEntityDef("Player");
        playerInfo.m_spawnVelocity = Vec2(0.f, 2000.f);
        factory->m_entitiesToSpawn.emplace_back(playerInfo);

        SpawnInfo asteroidInfo;
        asteroidInfo.m_spawnPos = Vec2(55.f, 30.f);
        asteroidInfo.m_def = factory->GetEntityDef("Asteroid");
        asteroidInfo.m_spawnVelocity = Vec2(0.f, 2000.f);
        factory->m_entitiesToSpawn.emplace_back(asteroidInfo);

        SpawnInfo earthInfo;
        earthInfo.m_spawnPos = Vec2(-150.f, 0.f);
        earthInfo.m_spawnVelocity = Vec2(0.f, 2000.f);
        earthInfo.m_def = factory->GetEntityDef("Earth");
        factory->m_entitiesToSpawn.emplace_back(earthInfo);

        earthInfo.m_spawnPos = Vec2(150.f, 0.f);
        earthInfo.m_spawnVelocity = Vec2(0.f, 2000.f - 150.f);
        factory->m_entitiesToSpawn.emplace_back(earthInfo);

        SpawnInfo sunInfo;
        sunInfo.m_spawnPos = Vec2(100000.f, 0.f);
        sunInfo.m_spawnVelocity = Vec2(0.f, 0.f);
        sunInfo.m_def = factory->GetEntityDef("Sun");
        factory->m_entitiesToSpawn.emplace_back(sunInfo);
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
