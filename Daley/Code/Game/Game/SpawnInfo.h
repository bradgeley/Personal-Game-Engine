// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <array>
#include <optional>



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
constexpr int MAX_SPAWN_TAGS = 8;



//----------------------------------------------------------------------------------------------------------------------
// Todo: add std::optional to most things in here?
//
struct SpawnInfo
{
	bool AddSpawnTag(Name tag);

	int m_randomSeed = 0; // Seed used for this enemy for the purposes of rolling random affixes, etc.

    Vec2  m_spawnPos;
    float m_spawnOrientation		= 0.f;
	float m_spawnScale				= 1.f;
	float m_spawnLifetime			= -1.f; // < 0 means infinite
	float m_spawnHealthMultiplier	= 1.f;
	float m_spawnSpeedMultiplier	= 1.f;
	float m_spawnExpMultiplier		= 1.f;
	int m_numAffixes = 0;
	std::optional<Rgba8> m_baseTint;
	Rgba8 m_outlineTint				= Rgba8::TransparentWhite;
	int m_numSpawnTags				= 0;
	std::array<Name, MAX_SPAWN_TAGS> m_spawnTags = { Name::Invalid };
	EntityDef const* m_def = nullptr;
};