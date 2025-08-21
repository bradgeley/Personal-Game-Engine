// Bradley Christensen - 2025
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SPhysics : public System
{
public:

    SPhysics(Name name = "Physics", Rgba8 const& debugTint = Rgba8::MediumSlateBlue) : System(name, debugTint) {};
    void Startup() override;
    void Run(SystemContext const& context) override;
    void Shutdown() override;

protected:

    bool DebugRenderPreventativePhysics(NamedProperties& args);
};