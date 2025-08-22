// Bradley Christensen - 2025
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SBackgroundMusic : public System
{
public:

    SBackgroundMusic(Name name = "BackgroundMusic", Rgba8 const& debugTint = Rgba8::Violet) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;
};
