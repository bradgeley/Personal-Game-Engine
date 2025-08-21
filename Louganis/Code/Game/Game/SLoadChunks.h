// Bradley Christensen - 2023
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SLoadChunks : public System
{
public:

    SLoadChunks(Name name = "LoadChunks", Rgba8 const& debugTint = Rgba8::Orange) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
