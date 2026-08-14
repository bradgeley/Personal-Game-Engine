// Bradley Christensen - 2022-2026
#include "SWaveSpawner.h"
#include "CTags.h"
#include "EntityDef.h"
#include "GameCommon.h"
#include "SCEntityFactory.h"
#include "SCWaves.h"
#include "SCWorld.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Math/Noise.h"
#include "Engine/Math/RandomNumberGenerator.h"



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Startup()
{
	AddReadDependencies<CTags, SCWorld>();
	AddWriteDependencies<SCWaves, SCEntityFactory, SCRandomNumberGenerator>();

	m_runWhilePaused = false;

	DevConsoleUtils::AddDevConsoleCommand("StartWaves", SWaveSpawner::StartWaves);
	DevConsoleUtils::AddDevConsoleCommand("GenerateWaves", SWaveSpawner::GenerateWaves, "seed", DevConsoleArgType::Int, "numWaves", DevConsoleArgType::Int);
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Shutdown() const
{
	DevConsoleUtils::RemoveDevConsoleCommand("StartWaves", SWaveSpawner::StartWaves);
	DevConsoleUtils::RemoveDevConsoleCommand("GenerateWaves", SWaveSpawner::GenerateWaves);
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::Run(SystemContext const& context) const
{
	// Read Dependencies
	SCWorld const& world = context.GetSingletonConst<SCWorld>();
	auto const& tagStorage = context.GetArrayStorageConst<CTags>();

	// Write Dependencies
	SCWaves& waves = context.GetSingleton<SCWaves>();
	SCEntityFactory& factory = context.GetSingleton<SCEntityFactory>();
	RandomNumberGenerator& rng = *context.GetSingleton<SCRandomNumberGenerator>().GetRNG();

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

			StartWave(waves, wave);

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

		float const& streamHealthMultiplier = stream.m_entityStream.m_healthMultiplier;
		float const& streamSpeedMultiplier = stream.m_entityStream.m_speedMultiplier;

		SpawnInfo spawnInfo;
		spawnInfo.m_def = EntityDef::GetEntityDef(stream.m_entityStream.m_entityName);

		CTags tags = spawnInfo.m_def->m_tags.has_value() ? *spawnInfo.m_def->m_tags : CTags();
		bool isBoss = tags.HasTag("boss");
		bool canHaveRarity = (waves.m_waveGenDef.m_waveGenModifiers.m_canBossesHaveRarity) ? true : !isBoss;

		float magicEnemyChance = stream.m_entityStream.m_magicEnemyChance + StaticGameSettings::s_baseMagicEnemyChance;
		float rareEnemyChance = stream.m_entityStream.m_rareEnemyChance + StaticGameSettings::s_baseRareEnemyChance;

		for (int spawnIndex = 0; spawnIndex < (int) numSpawns; ++spawnIndex)
		{
			if (canHaveRarity)
			{
				int entityPos = stream.m_numSpawned;
				float rarityRoll = Noise::GetNoiseZeroToOne1D(entityPos, stream.m_entityStream.m_id);

				if (rarityRoll < rareEnemyChance)
				{
					spawnInfo.m_outlineTint = StaticGameSettings::s_rareEnemyOutlineTint;
					spawnInfo.m_spawnHealthMultiplier = streamHealthMultiplier * StaticGameSettings::s_rareEnemyHealthMultiplier;
					spawnInfo.m_spawnSpeedMultiplier = streamSpeedMultiplier * StaticGameSettings::s_rareEnemySpeedMultiplier;
					spawnInfo.m_spawnScale = StaticGameSettings::s_rareEnemySizeMultiplier;
					spawnInfo.m_spawnTags[0] = "rare";
				}
				else if (rarityRoll < magicEnemyChance)
				{
					spawnInfo.m_outlineTint = StaticGameSettings::s_magicEnemyOutlineTint;
					spawnInfo.m_spawnHealthMultiplier = streamHealthMultiplier * StaticGameSettings::s_magicEnemyHealthMultiplier;
					spawnInfo.m_spawnSpeedMultiplier = streamSpeedMultiplier * StaticGameSettings::s_magicEnemySpeedMultiplier;
					spawnInfo.m_spawnScale = StaticGameSettings::s_magicEnemySizeMultiplier;
					spawnInfo.m_spawnTags[0] = "magic";
				}
				else // Normal
				{
					spawnInfo.m_outlineTint = Rgba8::TransparentWhite;
					spawnInfo.m_spawnHealthMultiplier = streamHealthMultiplier;
					spawnInfo.m_spawnSpeedMultiplier = streamSpeedMultiplier;
					spawnInfo.m_spawnScale = 1.f;
				}
			}

			spawnInfo.m_spawnPos = world.GetRandomSpawnLocation_SeededNoise(stream.m_numSpawned, stream.m_entityStream.m_id, rng);
			factory.m_entitiesToSpawn.push_back(spawnInfo);
			stream.m_numSpawned++;
		}

		if (stream.IsComplete())
		{
			waves.m_activeStreams.erase(waves.m_activeStreams.begin() + streamIndex);
			continue;
		}
	}

	waves.m_remainingEnemies = 0;
	for (auto it = context.Iterate<CTags>(); it.IsValid(); ++it)
	{
		CTags const& tags = tagStorage[it];
		if (tags.HasTag("enemy"))
		{
			waves.m_remainingEnemies++;
		}
	}

	if (waves.m_activeStreams.size() == 0 && waves.m_currentWaveIndex == waves.m_waves.size() && waves.m_remainingEnemies == 0)
	{
		// Waves are complete, including all active streams
		waves.m_wavesFinished = true;
	}
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

	waves.m_waveTimer = Timer(waves.m_secondsBetweenWaves, true);
	waves.m_waveTimer.ForceComplete(); // First wave starts immediately

	ASSERT_OR_DIE(waves.m_waveTimer.IsComplete(), "Timer did not complete.");

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SWaveSpawner::GenerateWaves(NamedProperties& args)
{
	int seed = args.Get("seed", 0);
	int numWaves = args.Get("numWaves", 0);

	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();

	GenerateWaves(waves, seed, numWaves);

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::StartWave(SCWaves& waves, Wave& wave)
{
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



//----------------------------------------------------------------------------------------------------------------------
static Name GetRandomEnemyWithTags(std::vector<Name> const& tags, RandomNumberGenerator& rng)
{
	std::vector<Name> matchingEnemies;
	if (EntityDef::GetAllEntityDefsWithTags(tags, matchingEnemies))
	{
		int randomIndex = rng.GetRandomIntInRange(0, (int) matchingEnemies.size() - 1);
		return matchingEnemies[randomIndex];
	}
	else
	{
		ERROR_AND_DIE("GetRandomEnemyWithTags: No entity defs found.");
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWaveSpawner::GenerateWaves(SCWaves& waves, int seed, int numWaves)
{
	RandomNumberGenerator rng(static_cast<size_t>(seed));

	// Static wave gen def for now, later get from a file
	waves.m_waveGenDef.m_numWaves = numWaves;
	waves.m_waveGenDef.m_seed = seed;
	waves.m_waveGenDef.m_randomWaves.push_back(RandomWaveStreamDef{ 10.f, 20, 25, { "ant", "small" }, 1.f });
	waves.m_waveGenDef.m_randomWaves.push_back(RandomWaveStreamDef{ 10.f, 5, 10, { "ant", "medium" },  0.5f });
	waves.m_waveGenDef.m_fixedWaves.push_back(FixedWaveStreamDef{ 10.f, 4, 5, "largeAnt", 1, false });
	waves.m_waveGenDef.m_fixedWaves.push_back(FixedWaveStreamDef{ 10.f, 4, 5, "ant", 20, false });
	waves.m_waveGenDef.m_fixedWaves.push_back(FixedWaveStreamDef{ 10.f, 14, -1, "bossAnt", 1, true });
	waves.m_waveGenDef.m_fixedWaves.push_back(FixedWaveStreamDef{ 30.f, 7, 7, "ant", 100, false});

	//waves.m_waveGenDef.m_randomWaves.push_back(RandomWaveStreamDef{ 10.f, 20, 25, { "boss", "ant" }, 1.f });

	// Modifiers
	waves.m_waveGenDef.m_waveGenModifiers.m_numEntitiesMultiplier = 1.f;
	waves.m_waveGenDef.m_waveGenModifiers.m_numEntitiesMultiplierIncreasePerWave = 0.5f;
	waves.m_waveGenDef.m_waveGenModifiers.m_waveSpawnRateMultiplier = 1.f;
	waves.m_waveGenDef.m_waveGenModifiers.m_healthMultiplierIncreasePerWave = 0.15f;
	waves.m_waveGenDef.m_waveGenModifiers.m_speedMultiplierIncreasePerWave = 0.02f;

	waves.m_waves.clear();

	// Seconds Between Waves
	float spawnRateMultiplier = MathUtils::Clamp(waves.m_waveGenDef.m_waveGenModifiers.m_waveSpawnRateMultiplier, 0.01f, 100.f);
	float oneOverSpawnRateMultiplier = 1.f / spawnRateMultiplier;
	waves.m_secondsBetweenWaves = 30.f * oneOverSpawnRateMultiplier;

	// Entity count multiplier
	float numEntitiesMultiplier = waves.m_waveGenDef.m_waveGenModifiers.m_numEntitiesMultiplier;

	int waveStreamsPushed = 0;

	// Waves
	for (int waveIndex = 0; waveIndex < numWaves; ++waveIndex)
	{
		Wave wave;

		// Figure out if there is a fixed wave at this index
		for (int fixedWaveIndex = 0; fixedWaveIndex < (int) waves.m_waveGenDef.m_fixedWaves.size(); ++fixedWaveIndex)
		{
			FixedWaveStreamDef const& fixedDef = waves.m_waveGenDef.m_fixedWaves[fixedWaveIndex];

			bool isMatchingWave = fixedDef.m_waveIndex == waveIndex;
			bool isRecurringWave = fixedDef.m_recurAfterNumWaves > 0 && waveIndex >= fixedDef.m_waveIndex && (waveIndex - fixedDef.m_waveIndex) % fixedDef.m_recurAfterNumWaves == 0;
			bool isGuaranteedLastWave = fixedDef.m_isGuaranteedLastWave && waveIndex == numWaves - 1;

			if (isMatchingWave || isRecurringWave || isGuaranteedLastWave)
			{
				WaveStream stream;
				stream.m_id = waveStreamsPushed++;
				stream.m_entityName = fixedDef.m_entityName;
				ASSERT_OR_DIE(EntityDef::GetEntityDef(stream.m_entityName) != nullptr, StringUtils::StringF("Invalid entity name in fixed wave def: %s", stream.m_entityName.ToCStr()).c_str());
				stream.m_numEntities = static_cast<int>(static_cast<float>(fixedDef.m_numEntities) * numEntitiesMultiplier);
				stream.m_numEntities = MathUtils::Max(1, stream.m_numEntities);
				stream.m_overTimeSeconds = fixedDef.m_overTimeSeconds * oneOverSpawnRateMultiplier;
				stream.m_overTimeSeconds = MathUtils::Max(0.f, stream.m_overTimeSeconds);
				stream.m_healthMultiplier = waves.m_waveGenDef.GetHealthScaling(waveIndex);
				stream.m_speedMultiplier = waves.m_waveGenDef.GetSpeedScaling(waveIndex);
				stream.m_magicEnemyChance = waves.m_waveGenDef.GetMagicEnemyChance(waveIndex);
				stream.m_rareEnemyChance = waves.m_waveGenDef.GetRareEnemyChance(waveIndex);
				wave.m_waveStreams.push_back(stream);
			}
		}

		if (!wave.m_waveStreams.empty())
		{
			waves.m_waves.push_back(wave);
			continue;
		}

		ASSERT_OR_DIE(waves.m_waveGenDef.m_randomWaves.size() > 0, StringUtils::StringF("No random waves defined in wave gen def, and there was not a fixed wave for index %i.", waveIndex).c_str());

		// No fixed wave, roll a random one

		// Choose which random wave definition to use based on weights
		float totalWeight = 0.f;
		for (RandomWaveStreamDef const& randomDef : waves.m_waveGenDef.m_randomWaves)
		{
			totalWeight += randomDef.m_weight;
		}

		int chosenRandomDefIndex = -1;
		float roll = rng.GetRandomFloatInRange(0.f, totalWeight);
		float cumulativeWeight = 0.f;
		for (int randomDefIndex = 0; randomDefIndex < (int) waves.m_waveGenDef.m_randomWaves.size(); ++randomDefIndex)
		{
			RandomWaveStreamDef const& randomDef = waves.m_waveGenDef.m_randomWaves[randomDefIndex];
			cumulativeWeight += randomDef.m_weight;

			if (roll <= cumulativeWeight)
			{
				chosenRandomDefIndex = randomDefIndex;
				break;
			}
		}

		ASSERT_OR_DIE(chosenRandomDefIndex != -1, "Failed to choose a random wave def.");

		RandomWaveStreamDef const& randomDef = waves.m_waveGenDef.m_randomWaves[chosenRandomDefIndex];
		WaveStream stream;
		stream.m_id = waveStreamsPushed++;
		stream.m_entityName = GetRandomEnemyWithTags(randomDef.m_enemyTags, rng);
		ASSERT_OR_DIE(EntityDef::GetEntityDef(stream.m_entityName) != nullptr, StringUtils::StringF("Invalid entity name in fixed wave def: %s", stream.m_entityName.ToCStr()).c_str());
		stream.m_numEntities = static_cast<int>(static_cast<float>(rng.GetRandomIntInRange(randomDef.m_minNumEntities, randomDef.m_maxNumEntities)) * numEntitiesMultiplier);
		stream.m_numEntities = MathUtils::Max(1, stream.m_numEntities);
		stream.m_overTimeSeconds = randomDef.m_overTimeSeconds * oneOverSpawnRateMultiplier;
		stream.m_overTimeSeconds = MathUtils::Max(0.f, stream.m_overTimeSeconds);
		stream.m_healthMultiplier = waves.m_waveGenDef.GetHealthScaling(waveIndex);
		stream.m_speedMultiplier = waves.m_waveGenDef.GetSpeedScaling(waveIndex);
		stream.m_magicEnemyChance = waves.m_waveGenDef.GetMagicEnemyChance(waveIndex);
		stream.m_rareEnemyChance = waves.m_waveGenDef.GetRareEnemyChance(waveIndex);
		wave.m_waveStreams.push_back(stream);
		waves.m_waves.push_back(wave);

		numEntitiesMultiplier += waves.m_waveGenDef.m_waveGenModifiers.m_numEntitiesMultiplierIncreasePerWave;
	}
}
