// Bradley Christensen - 2023
#pragma once
#include <string>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
enum class DevConsoleArgType
{
	None,
	Int,
	Float,
	Bool,
	String,
};



//----------------------------------------------------------------------------------------------------------------------
// Describes the intended usage of a dev console command, including arg names and types
// Used for suggesting arg names in the command prompt and making sure event args are stored as the right type.
//
struct DevConsoleCommandInfo
{
	DevConsoleCommandInfo(std::string const& commandName);

	std::string ToString() const;
	void AddArg(std::string const& argName, DevConsoleArgType argType);
	DevConsoleArgType GetArgType(std::string const& argName) const;
	static std::string ArgTypeToString(DevConsoleArgType argType);

	std::string m_commandName;
	std::vector<std::string> m_argNames;
	std::vector<DevConsoleArgType> m_argTypes;
};