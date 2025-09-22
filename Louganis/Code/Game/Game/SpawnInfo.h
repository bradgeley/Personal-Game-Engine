// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec2.h"



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
struct SpawnInfo
{
    Vec2 m_spawnPos;
    float m_spawnOrientation = 0.f;
	float m_spawnScale = 1.f;

    EntityDef const* m_def = nullptr;
};