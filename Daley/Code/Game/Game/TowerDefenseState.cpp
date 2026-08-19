// Bradley Christensen - 2022-2026
#include "TowerDefenseState.h"
#include "AllComponents.h"
#include "AllSystems.h"
#include "GameCommon.h"
#include "SCRunData.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Audio/AudioSystem.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Time/Clock.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
TowerDefenseState::TowerDefenseState()
{
	m_name = "TowerDefense";
}



//----------------------------------------------------------------------------------------------------------------------
void TowerDefenseState::Enter(NamedProperties const& props)
{
    GameState::Enter(props);

	DevConsoleUtils::AddDevConsoleCommand("Win", &TowerDefenseState::Win);
	DevConsoleUtils::AddDevConsoleCommand("Lose", &TowerDefenseState::Lose);

    StartGame(props);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerDefenseState::Exit(NamedProperties const& props)
{
    GameState::Exit(props);

	DevConsoleUtils::RemoveDevConsoleCommand("Win", &TowerDefenseState::Win);
	DevConsoleUtils::RemoveDevConsoleCommand("Lose", &TowerDefenseState::Lose);

    ShutdownGame();
}



//----------------------------------------------------------------------------------------------------------------------
void TowerDefenseState::Update(float)
{
    float deltaSeconds = m_clock->GetDeltaSecondsF();

	SCRunData& runData = g_ecs->GetSingleton<SCRunData>();
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();
	if (runData.m_health <= 0.f || waves.m_wavesFinished)
	{
        deltaSeconds = 0.f; // run paused if game is over due to loss
	}

    if (deltaSeconds == 0.f && g_input->WasKeyJustPressed(KeyCode::Enter))
    {
		deltaSeconds = 0.0166f; // force a single frame update while paused
    }

    g_ecs->RunFrame(deltaSeconds);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerDefenseState::Render() const
{
	// Empty, rendering happens in ECS systems
}
 


//----------------------------------------------------------------------------------------------------------------------
void TowerDefenseState::StartGame(NamedProperties const& inProps)
{
    ConfigureECS();

    // Seed the run
	SCRunData& runData = g_ecs->GetSingleton<SCRunData>();
	runData = inProps.Get<SCRunData>("runData", SCRunData());
	runData.m_gold = StaticGameSettings::s_baseGold;
	runData.m_health = StaticGameSettings::s_basePlayerHealth;
	runData.m_interestTimerSecondsRemaining = StaticGameSettings::s_baseInterestTimerSeconds;
	runData.m_numSoldTowers = 0;

	int numWaves = StaticGameSettings::s_numWavesInFirstMission + runData.m_missionIndex * StaticGameSettings::s_numWavesIncreasePerMission;
    int seed = runData.m_seed + runData.m_missionIndex;

    g_ecs->Startup();

    NamedProperties props;
    props.Set<int>("seed", seed);
	props.Set<int>("numWaves", numWaves);
	g_eventSystem->FireEvent("GenerateWaves", props);

	g_eventSystem->FireEvent("StartWaves", props);

	g_eventSystem->FireEvent("TogglePaused", props);
}



//----------------------------------------------------------------------------------------------------------------------
void TowerDefenseState::ShutdownGame()
{
    g_ecs->Shutdown();

    delete g_ecs;
    g_ecs = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
void TowerDefenseState::ConfigureECS()
{
    AdminSystemConfig ecsConfig;
    ecsConfig.m_maxDeltaSeconds = 1.f;
    ecsConfig.m_enableMultithreading = true;
    ecsConfig.m_autoMultithreadingEntityThreshold = 1;
    ecsConfig.m_systemSplittingEntityThreshold = 1;
    g_ecs = new AdminSystem(ecsConfig);

    //----------------------------------------------------------------------------------------------------------------------
    // COMPONENTS
    // 

    // Array components
    g_ecs->RegisterComponentArray<CAIController>();
    g_ecs->RegisterComponentArray<CAnimation>();
    g_ecs->RegisterComponentArray<CAttachment>();
    g_ecs->RegisterComponentArray<CCollision>();
    g_ecs->RegisterComponentArray<CCollisionEffect>();
    g_ecs->RegisterComponentArray<CDeath>();
    g_ecs->RegisterComponentArray<CHealth>();
    g_ecs->RegisterComponentArray<CLifetime>();
    g_ecs->RegisterComponentArray<CMovement>();
    g_ecs->RegisterComponentArray<CRender>();
    g_ecs->RegisterComponentArray<CTags>();
    g_ecs->RegisterComponentArray<CTime>();
    g_ecs->RegisterComponentArray<CTransform>();
    g_ecs->RegisterComponentArray<CEntityName>();

    // Map Components
    g_ecs->RegisterComponentMap<CAbility>();
    g_ecs->RegisterComponentMap<CProjectile>();
    g_ecs->RegisterComponentMap<CPlaceable>();

    // Singleton components
    g_ecs->RegisterComponentSingleton<SCCamera>();
    g_ecs->RegisterComponentSingleton<SCCollision>();
    g_ecs->RegisterComponentSingleton<SCDebug>();
    g_ecs->RegisterComponentSingleton<SCEntityFactory>();
    g_ecs->RegisterComponentSingleton<SCFloatingText>();
    g_ecs->RegisterComponentSingleton<SCFlowField>();
    g_ecs->RegisterComponentSingleton<SCGameState>();
    g_ecs->RegisterComponentSingleton<SCRunData>();
    g_ecs->RegisterComponentSingleton<SCLighting>();
    g_ecs->RegisterComponentSingleton<SCTime>();
    g_ecs->RegisterComponentSingleton<SCWorld>();
    g_ecs->RegisterComponentSingleton<SCWaves>();

    // Engine Proxies
    g_ecs->RegisterComponentSingleton<SCAssetManager>();
    g_ecs->RegisterComponentSingleton<SCAudioSystem>();
    g_ecs->RegisterComponentSingleton<SCEventSystem>();
    g_ecs->RegisterComponentSingleton<SCInputSystem>();
    g_ecs->RegisterComponentSingleton<SCRandomNumberGenerator>();
    g_ecs->RegisterComponentSingleton<SCRenderer>();
    g_ecs->RegisterComponentSingleton<SCWindow>();

    // Connect GameState to ECS world
    g_ecs->GetSingleton<SCGameState>().m_gameState = this;

    // Connect Engine to ECS world
    g_ecs->GetSingleton<SCAssetManager>().SetAssetManager(g_assetManager);
    g_ecs->GetSingleton<SCAudioSystem>().SetAudioSystem(g_audioSystem);
    g_ecs->GetSingleton<SCEventSystem>().SetEventSystem(g_eventSystem);
    g_ecs->GetSingleton<SCInputSystem>().SetInputSystem(g_input);
    g_ecs->GetSingleton<SCRenderer>().SetRenderer(g_renderer);
    g_ecs->GetSingleton<SCWindow>().SetWindow(g_window);
    g_ecs->GetSingleton<SCRandomNumberGenerator>().SetRNG(g_rng);

    int numRegisteredComponents = g_ecs->GetNumRegisteredComponents();
    constexpr int maxComponents = sizeof(size_t) * 8;
    DevConsoleUtils::LogWarning("Registered %i/%i components", numRegisteredComponents, maxComponents);


    //----------------------------------------------------------------------------------------------------------------------
    // SYSTEMS
    // 

    // Pre Physics
    g_ecs->RegisterSystem<SWorldTime>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SEntityTime>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SLifetime>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SWaveSpawner>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SAbility>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SEntityFactory>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SInput>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<STowerSpawner>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SDebugInput>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SWorld>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SBackgroundMusic>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SFlowField>((int) FramePhase::PrePhysics);
    g_ecs->RegisterSystem<SAIController>((int) FramePhase::PrePhysics);

    // Physics
    //SystemSubgraph& physics = g_ecs->CreateOrGetSystemSubgraph((int) FramePhase::Physics);
    //physics.m_timeStep = 0.005f;
    g_ecs->RegisterSystem<SMovement>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SPhysics>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SAttachment>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCollisionHash>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCollision>((int) FramePhase::Physics);
    g_ecs->RegisterSystem<SCollisionEffect>((int) FramePhase::Physics);

    // Post-Physics
    g_ecs->RegisterSystem<SProjectile>((int) FramePhase::PostPhysics);
    g_ecs->RegisterSystem<SHealth>((int) FramePhase::PostPhysics);
    g_ecs->RegisterSystem<SGoal>((int) FramePhase::PostPhysics);
    g_ecs->RegisterSystem<SCurrency>((int) FramePhase::PostPhysics);

    // Render
    g_ecs->RegisterSystem<SCopyTransform>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SCamera>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SInitView>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SMovementAnimation>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SAnimation>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SLighting>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderWorld>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SVisualEffects>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderEntities>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderAbilities>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderCollisionEffects>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderDiscs>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderHealthBars>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderStatusIcons>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SFloatingText>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderUI>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderHUD>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SRenderPopupOverlay>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SDebugRender>((int) FramePhase::Render);
    g_ecs->RegisterSystem<SDebugOverlay>((int) FramePhase::Render);

    // Random
    g_ecs->RegisterSystem<SSaveLoad>((int) FramePhase::PostRender);

    // Debug
    g_ecs->RegisterSystem<SDebugCommands>((int) FramePhase::PostRender);
    g_ecs->RegisterSystem<SSystemDebug>((int) FramePhase::PostRender);
    g_ecs->RegisterSystem<SDeath>((int) FramePhase::PostRender);
}



//----------------------------------------------------------------------------------------------------------------------
bool TowerDefenseState::Win(NamedProperties&)
{
	SCWaves& waves = g_ecs->GetSingleton<SCWaves>();
    waves.m_wavesFinished = true;
	waves.m_activeStreams.clear();

    for (auto it = g_ecs->IterateAll<CTags>(); it.IsValid(); ++it)
    {
		CTags const* tags = g_ecs->GetComponent<CTags>(it);
		if (tags && tags->HasTag("enemy"))
		{
			g_ecs->DestroyEntity(it.GetEntityID());
		}
    }

    waves.m_remainingEnemies = 0;

    SCRunData& runData = g_ecs->GetSingleton<SCRunData>();
    if (runData.m_health < 0.f)
    {
        runData.m_health = 1.f;
    }

    return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool TowerDefenseState::Lose(NamedProperties&)
{
    SCRunData& runData = g_ecs->GetSingleton<SCRunData>();

    runData.m_health = 0.f;

    return false;
}
