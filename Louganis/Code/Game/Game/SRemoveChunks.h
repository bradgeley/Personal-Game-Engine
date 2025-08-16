// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SRemoveChunks : public System
{
public:

    SRemoveChunks(std::string const& name = "RemoveChunks", Rgba8 const& debugTint = Rgba8::DarkRed) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
