// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Math/Vec2.h"
#include <array>



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_SPAWN_TAGS = 8;



//----------------------------------------------------------------------------------------------------------------------
struct SpawnInfo
{
    Vec2  m_spawnPos;
    float m_spawnOrientation		= 0.f;
	float m_spawnScale				= 1.f;
	float m_spawnLifetime			= -1.f; // < 0 means infinite
	float m_spawnHealthMultiplier	= 1.f;
	float m_spawnSpeedMultiplier	= 1.f;
	Rgba8 m_outlineTint				= Rgba8::TransparentWhite;
	std::array<Name, MAX_SPAWN_TAGS> m_spawnTags = { Name::Invalid };

    EntityDef const* m_def = nullptr;
};