// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Renderer/Rgba8.h"



//----------------------------------------------------------------------------------------------------------------------
extern class Application* g_app;



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

	static Rgba8 s_magicEnemyOutlineTint = Rgba8(0, 0, 255, 255);
	static Rgba8 s_rareEnemyOutlineTint = Rgba8(255, 255, 0, 255);
	static float s_magicEnemyHealthMultiplier = 1.5f;
	static float s_magicEnemySizeMultiplier = 1.25f;
	static float s_rareEnemyHealthMultiplier = 5.f;
	static float s_rareEnemySizeMultiplier = 1.75f;

    // Derived
    extern float s_burnDecayK;
	extern float s_oneOverBurnDecayK;
};