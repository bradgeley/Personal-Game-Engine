// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Rgba8.h"
#include <array>
#include <string>



//----------------------------------------------------------------------------------------------------------------------
extern class Application* g_app;



//----------------------------------------------------------------------------------------------------------------------
enum class FramePhase : int
{
    PrePhysics,
    Physics,
    PostPhysics,
    Render,
    PostRender,
};



//----------------------------------------------------------------------------------------------------------------------
enum class EDirection : int
{
    North,
    South,
    East,
    West,
    NorthEast,
    NorthWest,
    SouthEast,
    SouthWest,
};



//----------------------------------------------------------------------------------------------------------------------
enum class MetaAttribute : uint32_t
{
	Invalid = 0,
	Exp,
	SwirlLimit,
};
static std::array<std::string, 3> s_metaAttributeEnumStrings = { "Invalid", "Exp", "SwirlLimit" };



//----------------------------------------------------------------------------------------------------------------------
namespace StaticGameSettings
{
	// Rendering
	static constexpr float s_defaultPuddleRenderDepth		= 0.95f;
	static Rgba8		   s_magicEnemyOutlineTint			= Rgba8(0, 0, 255, 255);
	static Rgba8		   s_rareEnemyOutlineTint			= Rgba8(255, 255, 0, 255);

    // Static
    static constexpr float s_burnDecayedValueAfterOneSecond	= 0.65f;
	static constexpr double s_slowStatusTimeDilation		= 0.5;
	static constexpr double s_hasteStatusTimeDilation		= 1.5;
	static constexpr double s_vulnerabilityExponent			= 0.5f;
	static constexpr float s_burnSuppressionRatio			= 0.5f;	
	static constexpr float s_poisonSuppressionRatio			= 0.5f;	

	// Tower Settings
	static constexpr float s_baseCritMultiplier				= 2.f;
	static constexpr float s_baseChainChance				= 1.f;
	static constexpr float s_baseChainDistance				= 10.f;
	static constexpr float s_baseChainPayloadMulti			= 0.8f;
	static constexpr int   s_maxChainTargets				= 10; // Absolute maximum
	static constexpr int   s_flavorUnlockLimit				= 4; // max flavors that can be unlocked in 1 run
	static constexpr int   s_baseSwirlLimit					= 1; // max flavors that can be put into 1 tower baseline
	static constexpr int   s_absoluteSwirlLimit				= 3; // max flavors that can ever be put into 1 tower

	// Player Settings
	static constexpr float s_basePlayerHealth				= 100.f;
	static constexpr float s_basePlayerHealthRegen			= 0.1f;

	// Experience
	static constexpr double s_baseLevelExpRequirement		= 100.0;
	static constexpr double s_expRequirementExponent		= 1.15;
	static constexpr int    s_maxLevel						= 100;

	// Economy
	static constexpr float s_baseGold						= 100.f;
	static constexpr float s_baseIncome						= 50.f;
	static constexpr float s_baseIncomePerLevel				= 1.f;
	static constexpr float s_baseCreditLimit				= 0.f;
	static constexpr float s_baseSavingsInterestRate		= 0.01f;
	static constexpr float s_baseDebtInterestRate			= 0.5f;
	static constexpr float s_baseInterestTimerSeconds		= 30.f;
	static constexpr float s_baseSwirlCost					= 50.f;
	static constexpr int   s_baseSellMaximum				= 5;
	static constexpr float s_baseSellRefundRate				= 0.5f;
	static constexpr float s_baseCostRampPerTower			= 1.00f;

	// Difficulty Settings
	static constexpr int   s_maxEnemyAffixes				= 3;
	static constexpr float s_endlessEntityMultiplierIncreasePerWave = 1.1f;

	// Magic/Rare Enemy Settings
	static constexpr float s_baseMagicEnemyChance			= 0.05f;
	static constexpr float s_magicEnemyChanceIncreasePerWave= 0.0025f;
	static constexpr float s_magicEnemyHealthMultiplier		= 1.5f;
	static constexpr float s_magicEnemySizeMultiplier		= 1.1f;
	static constexpr float s_magicEnemySpeedMultiplier		= 0.9f;
	static constexpr float s_magicEnemyExpMultiplier		= 3.f;
	static constexpr int   s_magicEnemyBaseNumAffixes		= 1;

	static constexpr float s_baseRareEnemyChance			= 0.01f;
	static constexpr float s_rareEnemyChanceIncreasePerWave	= 0.001f;
	static constexpr float s_rareEnemyHealthMultiplier		= 5.f;
	static constexpr float s_rareEnemySizeMultiplier		= 1.25f;
	static constexpr float s_rareEnemySpeedMultiplier		= 0.75f;
	static constexpr float s_rareEnemyExpMultiplier			= 5.f;
	static constexpr int   s_rareEnemyBaseNumAffixes		= 2;

	// Enemy Affixes
	static constexpr float s_healingAuraPercentHealthPerSec	= 0.01f;

	// Campaign
	static constexpr int   s_numMissionsForVictory			= 9;
	static constexpr int   s_numWavesInFirstMission			= 10;
	static constexpr int   s_numWavesIncreasePerMission		= 2;
	static constexpr float s_enemyHealthIncreasePerMission	= 0.1f;

	static constexpr float s_maxTowerAttacksPerSecond		= 10.f;
	static constexpr float s_minTowerAttackCooldown			= 1.f / s_maxTowerAttacksPerSecond;

	static constexpr float s_minTimeDilation				= 0.125f / 16.f;
	static constexpr float s_maxTimeDilation				= 32.f;

	// Camera
	static constexpr float s_minCameraHeight				= 10.f;
	static constexpr float s_maxCameraHeight				= 1000.f;
	static constexpr float s_cameraZoomRatioPerStep			= 0.1f;
	static constexpr float s_baseCameraHeightInTiles		= 100.f;
	static constexpr float s_cameraHeightRatioPanSpeed		= 0.5f; // If camera is 10 tiles high, pan speed is 5 tiles per second if this value is 0.5. That way panning gets faster the more zoomed out you are.

    // Derived
    extern float s_burnDecayK;
	extern float s_oneOverBurnDecayK;
	extern uint64_t s_maximumExperience;
	std::array<uint64_t, s_maxLevel>& GetExpLookupTable();
};