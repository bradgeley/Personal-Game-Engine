// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include "EventCallbackFunction.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
namespace EventUtils
{
	void SubscribeEventCallbackFunction(Name eventName, EventCallbackFunction callbackFunc);
	void UnsubscribeEventCallbackFunction(Name eventName, EventCallbackFunction callbackFunc);

	int FireEvent(Name eventName, NamedProperties& args);
	int FireEvent(Name eventName);
}
