// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
class SSaveLoad : public System
{
public:

    SSaveLoad(Name name = "SaveLoad", Rgba8 const& debugTint = Rgba8::Black) : System(name, debugTint) {};
    void Startup() override;
    void Shutdown() const override;
    void Run(SystemContext const& context) const override;

	static bool SaveGame(NamedProperties& args);
	static bool LoadGame(NamedProperties& args);
};
