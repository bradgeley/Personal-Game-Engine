// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Time/Timer.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct EntityStream
{
	Name m_entityName			= Name::Invalid;
	int m_numEntities			= 0;
	float m_overTimeSeconds		= 0.f;
	int m_waveNumber			= 0;
};



//----------------------------------------------------------------------------------------------------------------------
struct ActiveStream
{
public:

	bool IsComplete() const { return m_numSpawned == m_entityStream.m_numEntities; }

public:

	EntityStream m_entityStream;
	int m_numSpawned = 0;
	Timer m_spawnTimer;
};



//----------------------------------------------------------------------------------------------------------------------
struct Wave
{
	std::vector<EntityStream> m_entityStreams;
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
	std::vector<ActiveStream> m_activeStreams;
};