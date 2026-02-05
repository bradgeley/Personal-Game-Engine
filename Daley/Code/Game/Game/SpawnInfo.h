// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
struct SpawnInfo
{
    Vec2  m_spawnPos;
    float m_spawnOrientation	= 0.f;
	float m_spawnScale			= 1.f;
	float m_spawnLifetime		= -1.f; // < 0 means infinite
	Rgba8 m_spawnTint			= Rgba8::White;

    EntityDef const* m_def = nullptr;
};