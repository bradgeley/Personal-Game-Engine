// Bradley Christensen - 2022-2023
#pragma once
#include <string>



struct NamedProperties;



typedef bool (*EventCallbackFunction)(NamedProperties& args);



//----------------------------------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction callbackFunc);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction callbackFunc);
void FireEvent(std::string const& eventName, NamedProperties& args);
void FireEvent(std::string const& eventName);