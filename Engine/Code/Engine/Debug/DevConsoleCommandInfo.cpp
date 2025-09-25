// Bradley Christensen - 2022-2025
#include "DevConsoleCommandInfo.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
DevConsoleCommandInfo::DevConsoleCommandInfo(Name commandName) : m_commandName(commandName)
{

}



//----------------------------------------------------------------------------------------------------------------------
DevConsoleCommandInfo::DevConsoleCommandInfo(Name commandName, Name argName, DevConsoleArgType argType) : m_commandName(commandName)
{
	AddArg(argName, argType); 
}



//----------------------------------------------------------------------------------------------------------------------
std::string DevConsoleCommandInfo::ToString() const
{
	std::string result = StringUtils::StringF("%s ", m_commandName.ToCStr());
	for (int argIndex = 0; argIndex < (int) m_argNames.size(); ++argIndex)
	{
		Name argName = m_argNames[argIndex];
		DevConsoleArgType argType = m_argTypes[argIndex];
		result.append(StringUtils::StringF("%s=%s", argName.ToCStr(), ArgTypeToString(argType).c_str()));

		if (argIndex < (int) m_argNames.size() - 1)
		{
			result.append(", ");
		}
	}
	return result;
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleCommandInfo::AddArg(Name argName, DevConsoleArgType argType)
{
	m_argNames.emplace_back(argName);
	m_argTypes.emplace_back(argType);
}



//----------------------------------------------------------------------------------------------------------------------
DevConsoleArgType DevConsoleCommandInfo::GetArgType(Name argName) const
{
	for (int i = 0; i < (int) m_argNames.size(); ++i)
	{
		if (argName == m_argNames[i])
		{
			return m_argTypes[i];
		}
	}
	return DevConsoleArgType::None;
}



//----------------------------------------------------------------------------------------------------------------------
std::string DevConsoleCommandInfo::ArgTypeToString(DevConsoleArgType argType)
{
	switch (argType)
	{
		case DevConsoleArgType::Int:
			return "Int";
		case DevConsoleArgType::Float:
			return "Float";
		case DevConsoleArgType::Bool:
			return "Bool";
		case DevConsoleArgType::String:
			return "String";
		case DevConsoleArgType::Vec2:
			return "Vec2";
		default:
			break;
	}
	return "";
}
