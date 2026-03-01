// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Time/Timer.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct WaveStream
{
	Name m_entityName			= Name::Invalid;
	int m_numEntities			= 0;
	float m_overTimeSeconds		= 0.f;
	int m_waveNumber			= 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct ActiveWaveStream
{
public:

	bool IsComplete() const { return m_numSpawned >= m_entityStream.m_numEntities; }

public:

	WaveStream m_entityStream;
	int m_numSpawned = 0;
	Timer m_spawnTimer;
};



//----------------------------------------------------------------------------------------------------------------------
struct Wave
{
	std::vector<WaveStream> m_waveStreams;
};



//----------------------------------------------------------------------------------------------------------------------
struct WaveStreamDef
{
	int m_minNumEntities = 5;
	int m_maxNumEntities = 10;
	std::vector<Name> m_enemyTags;
	float m_overTimeSeconds = 10.f;
};



//----------------------------------------------------------------------------------------------------------------------
// e.g. 20% chance for a wave to have "ant" "small" enemies, between 5 and 10 of them, spawning over 30 seconds
struct RandomWaveStreamDef : public WaveStreamDef
{
	float m_weight = 1.f;						// Affects the chance that this wave type will be picked over others in the level gen def.
};



//----------------------------------------------------------------------------------------------------------------------
// e.g. Guaranteed "AntGeneral" on wave 10, with 10 - 20 flying ants spawning over 30 seconds
// or Guaranteed "AntQueen" every 10 waves, starting on wave 30
struct FixedWaveStreamDef : public WaveStreamDef
{
	int m_waveIndex = 0;						// if -1, this will be ignored unless m_isGuaranteedLastWave is true, in which case this will only be the last wave.
	int m_recurAfterNumWaves = -1;				// -1, not recurring. Otherwise, recur every this many waves.
	bool m_isGuaranteedLastWave = false;		// If true, this will also be placed into the last wave slot guaranteed
};



//----------------------------------------------------------------------------------------------------------------------
// Wave gen modifiers are applied to waves to change how they are generated.
struct LevelWaveGenModifiers
{
	float m_numEntitiesMultiplier = 1.f;					// Multiplies the number of entities spawned in the wave by this amount.
	float m_numEntitiesMultiplierIncreasePerWave = 0.01f;	// Increase numEntitiesMultiplier by this amount per wave
	float m_waveSpawnRateMultiplier = 1.f;					// Multiplies the spawn rate (both time between waves and stream spawn rate) of waves by this amount (2.0 means twice as fast)
};



//----------------------------------------------------------------------------------------------------------------------
// Structure that defines how waves will be randomly generated for a level.
// e.g.
// 20% chance for 30-50 “small” and “ant” enemy
// 20 % chance for 10 - 20 “medium” and “ant” enemy
// 5 % chance for 1 “LootGoblin” enemy
// 50 % chance for “small” and “flying” enemies
// Guaranteed AntGeneral on wave 10, with 10 - 20 flying ants
// Guaranteed AntQueen on wave 15
// With 5 ant generals and 100 flying ants
//
// Modifiers:
// e.g.
// All monsters spawned increased by 50%
// All waves spawn 10% faster 
// Every wave, increase the number of monsters spawned by 1%
struct LevelWaveGenDef
{
	int m_seed = 0;
	int m_numWaves = 5;
	std::vector<RandomWaveStreamDef> m_randomWaves;
	std::vector<FixedWaveStreamDef> m_fixedWaves;
	LevelWaveGenModifiers m_waveGenModifiers;
};



//----------------------------------------------------------------------------------------------------------------------
class SCWaves
{
public:

	bool m_wavesStarted = false;
	bool m_wavesFinished = false;
	float m_waveTimerDuration = 30.f;
	Timer m_waveTimer;
	int m_currentWaveIndex = 0;
	std::vector<Wave> m_waves;
	std::vector<ActiveWaveStream> m_activeStreams;
};