// Bradley Christensen - 2022-2026
#include "SWorld.h"
#include "BiomeDef.h"
#include "CCollision.h"
#include "MapGeneratorDef.h"
#include "MapGenerator.h"
#include "SCEntityFactory.h"
#include "SCFlowField.h"
#include "SCRunData.h"
#include "SCWorld.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/ECS/SystemContext.h"



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Startup()
{
	AddWriteDependencies<SCWorld>();
	AddReadDependencies<CCollision>();

	DevConsoleUtils::AddDevConsoleCommand("GenerateMap", &SWorld::GenerateMap, "mapGenName", DevConsoleArgType::Name, "seed", DevConsoleArgType::UInt);

	SCRunData const& scRunData = g_ecs->GetSingleton<SCRunData>();
	RunData const& runData = *scRunData.m_data;
	MissionGenData const& missionGenData = runData.m_missionGenData[runData.m_missionIndex];

	NamedProperties props;
	props.Set<Name>("mapGenName", missionGenData.m_mapName);
	props.Set<Name>("biomeName", missionGenData.m_biomeName);
	props.Set<uint32_t>("seed", runData.m_seed + static_cast<uint32_t>(runData.m_missionIndex));
	GenerateMap(props);
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Run(SystemContext const& context) const
{
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	auto const& collisionStorage = context.GetArrayStorageConst<CCollision>();

	// Update the enemy counts for each tile, for the tower placement system to use

	scWorld.m_numEnemiesInTile.SetAll(0);

	for (auto it = context.Iterate<CCollision>(); it.IsValid(); ++it)
	{
		CCollision const& collision = collisionStorage[it];
		if (collision.m_spatialPartitionTileIndex == -1)
		{
			continue;
		}

		if (collision.m_collisionChannel == CollisionChannel::Enemy)
		{
			scWorld.m_numEnemiesInTile.GetRef(collision.m_spatialPartitionTileIndex) += 1;
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::Shutdown() const
{
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.Shutdown();

	DevConsoleUtils::RemoveDevConsoleCommand("GenerateMap", &SWorld::GenerateMap);

}



//----------------------------------------------------------------------------------------------------------------------
void SWorld::EndFrame() const
{
	SCWorld& scWorld = g_ecs->GetSingleton<SCWorld>();
	scWorld.m_solidnessOfPathTileChanged = false;
}



//----------------------------------------------------------------------------------------------------------------------
bool SWorld::GenerateMap(NamedProperties& params)
{
	Name mapGenName = params.Get<Name>("mapGenName", Name(""));
	Name biomeName = params.Get<Name>("biomeName", Name(""));
	uint32_t seed = params.Get<uint32_t>("seed", 0);

	MapGeneratorDef const* def = MapGeneratorDef::GetMapGeneratorDef(mapGenName);
	if (def == nullptr)
	{
		return false;
	}

	BiomeDef const* biomeDef = BiomeDef::GetBiomeDef(biomeName);
	if (biomeDef == nullptr)
	{
		biomeDef = BiomeDef::GetBiomeDef("forest");
	}

	g_ecs->DestroyAllEntities();

	MapGenerator mapGenerator;
	mapGenerator.Initialize(*def, *biomeDef, seed);

	SCWorld& world = g_ecs->GetSingleton<SCWorld>();
	mapGenerator.GenerateMap(world);

	SCEntityFactory& scEntityFactory = g_ecs->GetSingleton<SCEntityFactory>();
	scEntityFactory.m_towerPlacements.insert(scEntityFactory.m_towerPlacements.end(), world.m_generatedTowers.begin(), world.m_generatedTowers.end());

	SCFlowField& scFlowField = g_ecs->GetSingleton<SCFlowField>();
	scFlowField.m_toGoalFlowField.Reset();

	return false;
}
