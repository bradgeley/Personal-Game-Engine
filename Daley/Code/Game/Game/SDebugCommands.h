// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SDebugCommands : public System
{
public:

    SDebugCommands(Name name = "DebugCommands", Rgba8 const& debugTint = Rgba8::Magenta) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() override;
    void Run(SystemContext const& context) override;

protected:

    static bool Spawn(NamedProperties& args);
    static bool DumpEntityDebug(NamedProperties& args);
    static bool SetDebugPlacementEntity(NamedProperties& args);
};
