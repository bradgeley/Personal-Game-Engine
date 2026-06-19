// Bradley Christensen - 2022-2026
#pragma once



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

    // Derived
    extern float s_burnDecayK;
	extern float s_oneOverBurnDecayK;
};