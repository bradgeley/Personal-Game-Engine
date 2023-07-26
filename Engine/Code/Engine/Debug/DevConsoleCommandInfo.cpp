// Bradley Christensen - 2023
#include "DevConsoleCommandInfo.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
DevConsoleCommandInfo::DevConsoleCommandInfo(std::string const& commandName) : m_commandName(commandName)
{
	ToLower(m_commandName);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandInfo::AddArg(std::string const& argName, SupportedDevConsoleArgType argType)
{
	std::string lowerArgName = argName;
	ToLower(lowerArgName);
	m_argNames.emplace_back(lowerArgName);
	m_argTypes.emplace_back(argType);
}



//----------------------------------------------------------------------------------------------------------------------
SupportedDevConsoleArgType DevConsoleCommandInfo::GetArgType(std::string const& argName) const
{
	for (int i = 0; i < (int) m_argNames.size(); ++i)
	{
		if (argName == m_argNames[i])
		{
			return m_argTypes[i];
		}
	}
	return SupportedDevConsoleArgType::None;
}