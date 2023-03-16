// Bradley Christensen - 2023
#pragma once
#include "Engine/Core/EngineSubsystem.h"



//----------------------------------------------------------------------------------------------------------------------
struct DevConsoleConfig
{

};



//----------------------------------------------------------------------------------------------------------------------
// Dev Console
//
// Debug interface for calling functions in the game and engine.
//
class DevConsole : public EngineSubsystem
{
public:

	DevConsole(DevConsoleConfig const& config);
	virtual ~DevConsole() override = default;

	DevConsoleConfig const m_config;

protected:

};