// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
struct SpawnInfo
{
    Vec2  m_spawnPos;
    float m_spawnOrientation		= 0.f;
	float m_spawnScale				= 1.f;
	float m_spawnLifetime			= -1.f; // < 0 means infinite
	float m_spawnHealthMultiplier	= 1.f;
	float m_spawnSpeedMultiplier	= 1.f;

    EntityDef const* m_def = nullptr;
};