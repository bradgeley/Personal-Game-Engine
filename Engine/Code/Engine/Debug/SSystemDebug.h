// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/ECS/System.h"



struct NamedProperties;



//----------------------------------------------------------------------------------------------------------------------
// System that any game can add to their ECS to add DevConsole commands to enable/disable systems by name
//
class SSystemDebug : public System
{
public:

	SSystemDebug(Name name = "SystemDebug", Rgba8 const& debugTint = Rgba8::Magenta);
	virtual void Startup() override;
	virtual void Shutdown() override;

protected:

	bool ToggleSystem(NamedProperties& args);
	bool PrintAllSystems(NamedProperties& args);
};

