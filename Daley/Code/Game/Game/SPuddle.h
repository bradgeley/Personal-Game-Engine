// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



//----------------------------------------------------------------------------------------------------------------------
class SPuddle : public System
{
public:

    SPuddle(Name name = "Puddle", Rgba8 const& debugTint = Rgba8::Yellow) : System(name, debugTint) {};
    virtual void Startup() override;
    virtual void Run(SystemContext const& context) const override;
};
