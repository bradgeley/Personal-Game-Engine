// Bradley Christensen - 2023
#include "SDebugKeys.h"
#include "Engine/Input/InputSystem.h"
#include "Game/Game/Singletons/SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SDebugKeys::Run(SystemContext const& context)
{
    if (g_input->WasKeyJustReleased(' '))
    {
        g_ecs->DestroyEntity(0);
        SCEntityFactory* factory = g_ecs->GetComponent<SCEntityFactory>();
        SpawnInfo playerInfo;
        playerInfo.m_spawnPos = Vec2(50.f, 25.f);
        playerInfo.m_def = factory->GetEntityDef("Player");
        factory->m_entitiesToSpawn.emplace_back(playerInfo);
    }
}
