// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



class SCWaves;
struct NamedProperties;
struct Wave;



//----------------------------------------------------------------------------------------------------------------------
class SWaveSpawner : public System
{
public:

    SWaveSpawner(Name name = "WaveSpawner", Rgba8 const& debugTint = Rgba8::LightYellow) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

public:

    static bool StartWaves(NamedProperties& args);
    static bool GenerateWaves(NamedProperties& args);

public:

    static void StartWave(Wave& wave);
	static void GenerateWaves(SCWaves& waves, int seed, int numWaves);
};
