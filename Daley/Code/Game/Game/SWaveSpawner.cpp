// Bradley Christensen - 2022-2026
#include "SWaveSpawner.h"
#include "EntityDef.h"
#include "SCEntityFactory.h"
#include "SCWaves.h"
#include "SCWorld.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Startup()
{
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();

	WaveStream testStream;
	testStream.m_entityName = Name("ant");
	testStream.m_numEntities = 100;
	testStream.m_overTimeSeconds = 20.f;
	testStream.m_waveNumber = 0;

	WaveStream testStream2;
	testStream2.m_entityName = Name("pig");
	testStream2.m_numEntities = 10;
	testStream2.m_overTimeSeconds = 20.f;
	testStream2.m_waveNumber = 0;

	Wave testWave;
	testWave.m_waveStreams.push_back(testStream);
	testWave.m_waveStreams.push_back(testStream2);

	waves.m_currentWaveIndex = 0;
	waves.m_waves.push_back(testWave);
	waves.m_waves.push_back(testWave);
	waves.m_waves.push_back(testWave);

	DevConsoleUtils::AddDevConsoleCommand("StartWaves", SWaveSpawner::StartWaves);
	DevConsoleUtils::AddDevConsoleCommand("GenerateWaves", SWaveSpawner::GenerateWaves, "seed", DevConsoleArgType::Int, "numWaves", DevConsoleArgType::Int);
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

	if (waves.m_currentWaveIndex < waves.m_waves.size())
	{
		int numWavesStarted = waves.m_waveTimer.UpdateAndCount(context.m_deltaSeconds);
		for (int waveIndex = 0; waveIndex < numWavesStarted; ++waveIndex)
		{
			int currentWaveIndex = waves.m_currentWaveIndex + waveIndex;
			Wave& wave = waves.m_waves[currentWaveIndex];

			StartWave(wave);

			waves.m_currentWaveIndex++;

			if (waves.m_currentWaveIndex >= (int) waves.m_waves.size())
			{
				// no more waves
				waves.m_waveTimer.ForceComplete();
			}
		}
	}

	// Update active streams
	for (int streamIndex = (int) waves.m_activeStreams.size() - 1; streamIndex >= 0; --streamIndex)
	{
		ActiveWaveStream& stream = waves.m_activeStreams[streamIndex];
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

	if (waves.m_activeStreams.size() == 0 && waves.m_currentWaveIndex == waves.m_waves.size())
	{
		// Waves are complete, including all active streams
		waves.m_wavesFinished = true;
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Shutdown()
{
	DevConsoleUtils::RemoveDevConsoleCommand("StartWaves", SWaveSpawner::StartWaves);
	DevConsoleUtils::RemoveDevConsoleCommand("GenerateWaves", SWaveSpawner::GenerateWaves);
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
	waves.m_waveTimer.ForceComplete(); // First wave starts immediately

	ASSERT_OR_DIE(waves.m_waveTimer.IsComplete(), "Timer did not complete.");

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SWaveSpawner::GenerateWaves(NamedProperties& args)
{
	int seed = args.Get("seed", 0);
	int numWaves = args.Get("numWaves", 0);

	// Static wave gen def
	LevelWaveGenDef waveGenDef;
	waveGenDef.m_numWaves = numWaves;
	waveGenDef.m_seed = seed;

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::StartWave(Wave& wave) const
{
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();

	for (WaveStream const& stream : wave.m_waveStreams)
	{
		if (stream.m_numEntities <= 0)
		{
			continue;
		}

		ActiveWaveStream activeStream;
		activeStream.m_entityStream = stream;
		activeStream.m_numSpawned = 0;
		activeStream.m_spawnTimer = Timer(stream.m_overTimeSeconds / stream.m_numEntities, true);
		activeStream.m_spawnTimer.ForceComplete(); // First wave starts immediately
		waves.m_activeStreams.push_back(activeStream);
	}
}
