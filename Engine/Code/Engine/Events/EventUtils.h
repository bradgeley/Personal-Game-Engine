// Bradley Christensen - 2022-2023
#pragma once
#include <string>



struct NamedProperties;



typedef bool (*EventCallbackFunction)(NamedProperties& args);



//----------------------------------------------------------------------------------------------------------------------
namespace EventUtils
{
	void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction callbackFunc);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction callbackFunc);

	int FireEvent(std::string const& eventName, NamedProperties& args);
	int FireEvent(std::string const& eventName);
}
