// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Rgba8.h"
#include <array>



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
namespace StaticGameSettings
{
    // Static
    static constexpr float s_burnDecayedValueAfterOneSecond	= 0.75f;
	static constexpr double s_slowStatusTimeDilation		= 0.5;
	static constexpr double s_hasteStatusTimeDilation		= 1.5;
    static constexpr float s_defaultCollisionEffectDepth	= 0.95f;

	static constexpr float s_baseMagicEnemyChance			= 0.05f;
	static constexpr float s_baseRareEnemyChance			= 0.01f;
	static Rgba8		   s_magicEnemyOutlineTint			= Rgba8(0, 0, 255, 255);
	static Rgba8		   s_rareEnemyOutlineTint			= Rgba8(255, 255, 0, 255);
	static constexpr float s_magicEnemyHealthMultiplier		= 1.5f;
	static constexpr float s_magicEnemySizeMultiplier		= 1.25f;
	static constexpr float s_magicEnemySpeedMultiplier		= 0.9f;
	static constexpr float s_rareEnemyHealthMultiplier		= 5.f;
	static constexpr float s_rareEnemySizeMultiplier		= 1.75f;
	static constexpr float s_rareEnemySpeedMultiplier		= 0.75f;

	static constexpr float s_basePlayerHealth				= 100.f;
	static constexpr float s_basePlayerHealthRegen			= 0.1f;
	static constexpr float s_baseGold						= 100.f;
	static constexpr float s_baseCreditLimit				= 0.f;
	static constexpr float s_baseSavingsInterestRate		= 0.08f;
	static constexpr float s_baseDebtInterestRate			= 0.5f;
	static constexpr float s_baseInterestTimerSeconds		= 30.f;
	static constexpr float s_baseSellRefundRate				= 0.5f;
	static constexpr int   s_baseSellMaximum				= 5;

	static constexpr double s_baseLevelExpRequirement		= 100.0;
	static constexpr double s_expRequirementExponent		= 1.15;
	static constexpr int    s_maxLevel						= 100;

	static constexpr int   s_numMissionsForVictory			= 9;
	static constexpr int   s_numWavesInFirstMission			= 10;
	static constexpr int   s_numWavesIncreasePerMission		= 2;
	static constexpr float s_enemyHealthIncreasePerMission	= 0.1f;

	static constexpr float s_minTimeDilation				= 0.25f;
	static constexpr float s_maxTimeDilation				= 4.f;

    // Derived
    extern float s_burnDecayK;
	extern float s_oneOverBurnDecayK;
	extern uint64_t s_maximumExperience;
	std::array<uint64_t, s_maxLevel>& GetExpLookupTable();
};