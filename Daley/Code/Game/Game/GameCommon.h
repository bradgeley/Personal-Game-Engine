// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Rgba8.h"



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
    static constexpr float s_burnDecayedValueAfterOneSecond = 0.75f;
	static constexpr double s_slowStatusTimeDilation = 0.5;
    static constexpr float s_defaultCollisionEffectDepth = 0.95f;

	static float s_baseMagicEnemyChance         = 0.05f;
	static float s_baseRareEnemyChance          = 0.01f;
	static Rgba8 s_magicEnemyOutlineTint        = Rgba8(0, 0, 255, 255);
	static Rgba8 s_rareEnemyOutlineTint         = Rgba8(255, 255, 0, 255);
	static float s_magicEnemyHealthMultiplier   = 1.5f;
	static float s_magicEnemySizeMultiplier     = 1.25f;
	static float s_magicEnemySpeedMultiplier    = 0.9f;
	static float s_rareEnemyHealthMultiplier    = 5.f;
	static float s_rareEnemySizeMultiplier      = 1.75f;
	static float s_rareEnemySpeedMultiplier     = 0.75f;

	static float s_basePlayerHealth				= 100.f;
	static float s_basePlayerHealthRegen		= 0.1f;
	static float s_baseGold						= 100.f;
	static float s_baseSavingsInterestRate      = 0.01f;
	static float s_baseDebtInterestRate         = 0.02f;
	static float s_baseInterestTimerSeconds     = 30.f;

    // Derived
    extern float s_burnDecayK;
	extern float s_oneOverBurnDecayK;
};