﻿// Bradley Christensen - 2022-2025
#include "DevConsoleUtils.h"
#include "DevConsole.h"
#include "Engine/Events/EventUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::Log(Rgba8 tint, char const* line)
{
	if (g_devConsole)
	{
		g_devConsole->AddLine(std::string(line), tint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::LogSuccess(char const* line)
{
	if (g_devConsole)
	{
		g_devConsole->AddLine(std::string(line), g_devConsole->GetConfig().m_successTint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::LogWarning(char const* line)
{
	if (g_devConsole)
	{
		g_devConsole->AddLine(std::string(line), g_devConsole->GetConfig().m_warningTint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::LogError(char const* line)
{
	if (g_devConsole)
	{
		g_devConsole->AddLine(std::string(line), g_devConsole->GetConfig().m_errorTint);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::AddCommandInfo(Name eventName, Name argName, DevConsoleArgType argType)
{
	if (g_devConsole)
	{
		g_devConsole->AddDevConsoleCommandInfo(eventName, argName, argType);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::RemoveCommandInfo(Name eventName)
{
	if (g_devConsole)
	{
		g_devConsole->RemoveDevConsoleCommandInfo(eventName);
	}
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::AddDevConsoleCommand(Name eventName, EventCallbackFunction function)
{
	EventUtils::SubscribeEventCallbackFunction(eventName, function);
}



//----------------------------------------------------------------------------------------------------------------------
void DevConsoleUtils::AddDevConsoleCommand(Name eventName, EventCallbackFunction function, Name argName, DevConsoleArgType argType)
{
	EventUtils::SubscribeEventCallbackFunction(eventName, function);
	if (g_devConsole)
	{
		DevConsoleCommandInfo commandInfo(eventName, argName, argType);
		g_devConsole->AddDevConsoleCommandInfo(commandInfo);
	}
}
