// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SUnloadChunks : public System
{
public:

    SUnloadChunks(std::string const& name = "UnloadChunks", Rgba8 const& debugTint = Rgba8::DarkOrange) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
