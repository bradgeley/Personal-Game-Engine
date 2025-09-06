// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Debug/DevConsoleArg.h"
#include <string>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// Describes the intended usage of a dev console command, including arg names and types
// Used for suggesting arg names in the command prompt and making sure event args are stored as the right type.
//
struct DevConsoleCommandInfo
{
public:

	DevConsoleCommandInfo(Name commandName);
	DevConsoleCommandInfo(Name commandName, Name argName, DevConsoleArgType argType);

	std::string ToString() const;
	void AddArg(Name argName, DevConsoleArgType argType);
	DevConsoleArgType GetArgType(Name argName) const;
	static std::string ArgTypeToString(DevConsoleArgType argType);

public:

	Name m_commandName;
	std::vector<Name> m_argNames;
	std::vector<DevConsoleArgType> m_argTypes;
};