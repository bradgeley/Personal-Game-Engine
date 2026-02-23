// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;
struct Wave;



//----------------------------------------------------------------------------------------------------------------------
class SWaveSpawner : public System
{
public:

    SWaveSpawner(Name name = "WaveSpawner", Rgba8 const& debugTint = Rgba8::LightYellow) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

public:

    static bool StartWaves(NamedProperties& args);
    void StartWave(Wave& wave) const;
};
