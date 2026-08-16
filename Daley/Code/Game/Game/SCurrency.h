// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SCurrency : public System
{
public:

    SCurrency(Name name = "Currency", Rgba8 const& debugTint = Rgba8::Blue) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

	static bool GrantGold(NamedProperties& params);
	static bool GrantSells(NamedProperties& params);
};
