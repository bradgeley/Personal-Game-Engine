// Bradley Christensen - 2023
#include "DevConsoleCommandInfo.h"
#include "Engine/Core/StringUtils.h"



//----------------------------------------------------------------------------------------------------------------------
DevConsoleCommandInfo::DevConsoleCommandInfo(std::string const& commandName) : m_commandName(commandName)
{

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
			break;
		case DevConsoleArgType::Float:
			return "Float";
			break;
		case DevConsoleArgType::Bool:
			return "Bool";
			break;
		case DevConsoleArgType::String:
			return "String";
			break;
		default:
			break;
	}
	return "";
}
