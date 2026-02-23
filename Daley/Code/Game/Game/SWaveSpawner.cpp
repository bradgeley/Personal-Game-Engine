// Bradley Christensen - 2022-2026
#include "SWaveSpawner.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "SCWaves.h"
#include "SCWorld.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Startup()
{
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();

	EntityStream testStream;
	testStream.m_entityName = Name("ant");
	testStream.m_numEntities = 100;
	testStream.m_overTimeSeconds = 10.f;
	testStream.m_waveNumber = 0;

	Wave testWave;
	testWave.m_entityStreams.push_back(testStream);

	waves.m_currentWaveIndex = 0;
	waves.m_waves.push_back(testWave);

	DevConsoleUtils::AddDevConsoleCommand("StartWaves", SWaveSpawner::StartWaves);
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Run(SystemContext const& context)
{
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();
	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	SCEntityFactory& factory = g_ecs->GetSingleton<SCEntityFactory>();

	if (waves.m_wavesFinished || !waves.m_wavesStarted)
	{
		return;
	}

	int numWavesStarted = waves.m_waveTimer.UpdateAndCount(context.m_deltaSeconds);
	for (int waveIndex = 0; waveIndex < numWavesStarted; ++waveIndex)
	{
		int currentWaveIndex = waves.m_currentWaveIndex + waveIndex;
		Wave& wave = waves.m_waves[currentWaveIndex];

		StartWave(wave);
	}

	// Update active streams
	for (int streamIndex = (int) waves.m_activeStreams.size() - 1; streamIndex >= 0; --streamIndex)
	{
		ActiveStream& stream = waves.m_activeStreams[streamIndex];
		int remainingSpawns = stream.m_entityStream.m_numEntities - stream.m_numSpawned;
		int numSpawns = stream.m_spawnTimer.UpdateAndCount(context.m_deltaSeconds);
		numSpawns = MathUtils::Min(numSpawns, remainingSpawns);

		SpawnInfo spawnInfo;
		spawnInfo.m_def = EntityDef::GetEntityDef(stream.m_entityStream.m_entityName);

		for (int spawnIndex = 0; spawnIndex < (int) numSpawns; ++spawnIndex)
		{
			spawnInfo.m_spawnPos = world.GetRandomSpawnLocation();
			factory.m_entitiesToSpawn.push_back(spawnInfo);
			stream.m_numSpawned++;
		}

		if (stream.IsComplete())
		{
			waves.m_activeStreams.erase(waves.m_activeStreams.begin() + streamIndex);
			continue;
		}
	}

	// check if waves are complete
	if (waves.m_activeStreams.size() == 0 && waves.m_currentWaveIndex == waves.m_waves.size() - 1)
	{
		waves.m_wavesFinished = true;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("StartWaves", SWaveSpawner::StartWaves);
}



//----------------------------------------------------------------------------------------------------------------------
bool SWaveSpawner::StartWaves(NamedProperties&)
{
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();

	if (waves.m_waves.size() == 0)
	{
		return false;
	}

	waves.m_activeStreams.clear();
	waves.m_wavesStarted = true;
	waves.m_wavesFinished = false;
	waves.m_currentWaveIndex = 0;

	waves.m_waveTimer = Timer(waves.m_waveTimerDuration, true);
	waves.m_waveTimer.ForceComplete(); // Force complete so that the first wave starts immediately

	ASSERT_OR_DIE(waves.m_waveTimer.IsComplete(), "Timer did not complete.");

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::StartWave(Wave& wave) const
{
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();

	for (EntityStream const& stream : wave.m_entityStreams)
	{
		if (stream.m_numEntities <= 0)
		{
			continue;
		}

		ActiveStream activeStream;
		activeStream.m_entityStream = stream;
		activeStream.m_numSpawned = 0;
		activeStream.m_spawnTimer = Timer(stream.m_overTimeSeconds / stream.m_numEntities, true);
		activeStream.m_spawnTimer.ForceComplete(); // Force complete so that the first spawn happens immediately
		waves.m_activeStreams.push_back(activeStream);
	}
}
