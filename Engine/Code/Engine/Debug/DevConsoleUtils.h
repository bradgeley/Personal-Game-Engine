// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "Engine/Debug/DevConsoleArg.h"
#include "Engine/Events/EventCallbackFunction.h"
#include "Engine/Renderer/Rgba8.h"
#include <cstdio>



//----------------------------------------------------------------------------------------------------------------------
namespace DevConsoleUtils
{
	void Log(Rgba8 tint, char const* line);
	void LogSuccess(char const* line);
	void LogWarning(char const* line);
	void LogError(char const* line);

	template<typename ...Args>
	void Log(Rgba8 tint, char const* format, Args... args);

	template<typename ...Args>
	void LogSuccess(char const* format, Args... args);

	template<typename ...Args>
	void LogWarning(char const* format, Args... args);

	template<typename ...Args>
	void LogError(char const* format, Args... args);

	void AddCommandInfo(Name eventName, Name argName, DevConsoleArgType argType);
	void RemoveCommandInfo(Name eventName);
	void AddDevConsoleCommand(Name eventName, EventCallbackFunction function);
	void AddDevConsoleCommand(Name eventName, EventCallbackFunction function, Name argName, DevConsoleArgType argType);
	void AddDevConsoleCommand(Name eventName, EventCallbackFunction function, Name arg1Name, DevConsoleArgType arg1Type, Name arg2Name, DevConsoleArgType arg2Type);
	void RemoveDevConsoleCommand(Name eventName, EventCallbackFunction function);
};



//----------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
void DevConsoleUtils::Log(Rgba8 tint, char const* format, Args ...args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	Log(tint, buffer);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
void DevConsoleUtils::LogSuccess(char const* format, Args ...args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	LogSuccess(buffer);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
void DevConsoleUtils::LogWarning(char const* format, Args ...args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	LogWarning(buffer);
}



//----------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
void DevConsoleUtils::LogError(char const* format, Args ...args)
{
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), format, args...);
	LogError(buffer);
}
