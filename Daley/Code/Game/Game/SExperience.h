// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SExperience : public System
{
public:

    SExperience(Name name = "Experience", Rgba8 const& debugTint = Rgba8::Yellow) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

	static bool GrantExp(NamedProperties& params);
};
