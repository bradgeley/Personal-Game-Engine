// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SDebugCommands : public System
{
public:

    SDebugCommands(Name name = "DebugCommands", Rgba8 const& debugTint = Rgba8::Magenta) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

protected:

    static bool Goto(NamedProperties& args);
	static bool Ghost(NamedProperties& args);
    static bool SetSeed(NamedProperties& args);
    static bool SlowPlayer(NamedProperties& args);
    static bool SetDebugTileDef(NamedProperties& args);
    static bool SetTimeOfDay(NamedProperties& args);
};
